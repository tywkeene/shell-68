#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <builtin/hexedit.h>
#include <sh/api.h>
#include <sh/builtin.h>
#include <sh/debug.h>
#include <sh/errcode.h>
#include <sh/heap.h>
#include <sh/print.h>
#include <sh/status_led.h>
#include <shell.h>
#include <sync/mutex.h>
#include <sys/status_dword.h>
#include <util/misc.h>
#include <util/string.h>

typedef int (*builtin_fn_t)(int argc, const char **argv);

extern int builtin_fn_hexedit(int argc, const char **argv);
static int builtin_fn_help(int argc, const char **argv);

enum {
  BUILTIN_HALT,
  BUILTIN_HELP,
  BUILTIN_MEMDUMP,
  BUILTIN_PANIC,
  BUILTIN_MALLOC,
  BUILTIN_BLOCK_INFO,
  BUILTIN_HEAP_INFO,
  BUILTIN_LED_TOGGLE,
  BUILTIN_TASKS,
  BUILTIN_HEX,
  BUILTIN_STATUS,
  BUILTIN_MUTEX,
  BUILTIN_LS,
  BUILTIN_RUN,
  NUM_BUILTINS,
};

static int builtin_fn_run(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  if (!sys_get_status(SYS_SH_DISK_MASK)) {
    sh_errorln(DEBUG_CALL_INFO, "disk not initialized");
    return ERR_CODE_ERROR;
  }

  const char *filename = "/starter_c.bin";
  uint32_t size = sys_file_size(filename);
  uint8_t *p = (uint8_t *)sh_calloc(size + 8, sizeof(uint8_t));
  return sys_execute(filename, p);
}

static int builtin_fn_ls(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  if (!sys_get_status(SYS_SH_DISK_MASK)) {
    sh_errorln(DEBUG_CALL_INFO, "disk not initialized");
    return ERR_CODE_ERROR;
  }
  sys_list_dir("/");
  return ERR_CODE_OK;
}

static int builtin_fn_mutex(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);

  sh_mutex_t m;
  SH_INIT_MUTEX(m);

  sh_println("acquiring mutex");
  sh_mutex_acquire(&m);
  sh_debug_mutex_state(&m);
  sh_println("releasing mutex");
  sh_mutex_release(&m);
  return ERR_CODE_OK;
}

static int builtin_fn_status(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  sys_print_status_dword();
  return ERR_CODE_OK;
}

static int builtin_fn_tasks(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  sh_print_task_list();
  return ERR_CODE_OK;
}

static int builtin_fn_heap_info(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  sh_heap_debug_info();
  return ERR_CODE_OK;
}

static int builtin_fn_panic(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  return ERR_CODE_PANIC;
}

static int builtin_fn_led_toggle(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  sh_status_led_toggle();
  return ERR_CODE_OK;
}

static int builtin_fn_malloc(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);

  char input[4];
  sh_prompt_input("num bytes: ", input, 4);
  uint32_t size = strtol(input, NULL, 10);

  sh_println("allocating %d bytes", size);
  char *data = sh_malloc(size);
  if (data == NULL)
    return ERR_CODE_PANIC;

  sh_println("sh_malloc(%d) returned pointer: 0x%08x", size, (uint32_t *)data);
  memset(data, 'x', size);

  uint32_t *p = (uint32_t *)data;
  for (uint32_t i = 0; i < (size / 4); i++) {
    sh_printf("[0x%08x]: %#04x %#04x %#04x %#04x ", (void *)p,
              (uint16_t)(*p >> 24), (uint16_t)(*p >> 16), (uint16_t)(*p >> 8),
              (uint16_t)(*p));
    sh_println("| %c %c %c %c |", ascii_or_dot((uint16_t)(*p >> 24)),
               ascii_or_dot((uint16_t)(*p >> 16)),
               ascii_or_dot((uint16_t)(*p >> 8)), ascii_or_dot((uint16_t)(*p)));
    p++;
  }
  return ERR_CODE_OK;
}

static int builtin_fn_block_info(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);

  sh_println("%s", sh_heap_header_str());
  sh_debug_mem_blocks_info();
  return ERR_CODE_OK;
}

static int builtin_fn_memdump(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);

  const uint32_t USER_RAM_START = 0x00001000;
  const uint32_t USER_RAM_END = 0x000FFFFF;
  char start_str[9];
  char end_str[9];

_get_input:
  memset(start_str, 0, 9);
  memset(end_str, 0, 9);

  sh_println("user ram is 0x%08x - 0x%08x", (void *)USER_RAM_START,
             (void *)USER_RAM_END);
  sh_println("enter start and end addr, or q<ENTER> to exit");

  sh_prompt_input("start addr: 0x", start_str, 9);
  if (start_str[0] == 'q') {
    return ERR_CODE_EXIT;
  }

  sh_prompt_input("  end addr: 0x", end_str, 9);
  if (end_str[0] == 'q') {
    return ERR_CODE_EXIT;
  }
  sh_println("");

  uint32_t *start = (uint32_t *)strtol(start_str, NULL, 16);
  uint32_t *end = (uint32_t *)strtol(end_str, NULL, 16);

  if (*start >= *end) {
    sh_println("start addr must be less than end addr (0x%08x > 0x%08x)",
               (void *)start, (void *)end);
    goto _get_input;
  }
  sh_hex_dump(start, end);
  return ERR_CODE_OK;
}

noreturn int builtin_fn_halt(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  sh_get_api()->sys_halt();
  __builtin_unreachable();
}

static const struct {
  const char *str;
  const char *help;
  builtin_fn_t fn;
} builtin_lut[NUM_BUILTINS] = {
    [BUILTIN_HALT] = {"halt", "halt the machine", builtin_fn_halt},
    [BUILTIN_HELP] = {"help", "print help messages of all commands",
                      builtin_fn_help},
    [BUILTIN_MEMDUMP] = {"memdump", "print a hexdump of a region in memory",
                         builtin_fn_memdump},
    [BUILTIN_PANIC] = {"panic", "force a panic and halt the machine",
                       builtin_fn_panic},
    [BUILTIN_MALLOC] = {"malloc",
                        "allocate some memory and print pointer information",
                        builtin_fn_malloc},
    [BUILTIN_BLOCK_INFO] = {"blocks", "dump all managed memory block info",
                            builtin_fn_block_info},
    [BUILTIN_HEAP_INFO] = {"heap", "dump debug heap info",
                           builtin_fn_heap_info},
    [BUILTIN_LED_TOGGLE] = {"led", "toggle the system status led",
                            builtin_fn_led_toggle},
    [BUILTIN_TASKS] = {"tasks", "print list of currently running tasks",
                       builtin_fn_tasks},
    [BUILTIN_HEX] = {"hex", "a simple hex editor", builtin_fn_hexedit},
    [BUILTIN_STATUS] = {"status", "print the state of the system status dword",
                        builtin_fn_status},
    [BUILTIN_MUTEX] = {"mutex", "test mutex implementation", builtin_fn_mutex},
    [BUILTIN_LS] = {"ls", "print current directory listing", builtin_fn_ls},
    [BUILTIN_RUN] = {"run", "run a binary file", builtin_fn_run},
};

static int builtin_fn_help(int argc, const char **argv) {
  _arg_not_used(argc);
  _arg_not_used(argv);
  for (int i = 0; i < NUM_BUILTINS; i++) {
#if defined(_SH_DEBUG_CMDLINE)
    sh_debugln(DEBUG_CALL_INFO, "[addr: 0x%p]->  %s : %s", builtin_lut[i].fn,
               builtin_lut[i].str, builtin_lut[i].help);
#else
    sh_println("    %s : %s", builtin_lut[i].str, builtin_lut[i].help);
#endif
  }
  return ERR_CODE_OK;
}

int execute_builtin(char *in) {
  if (!strlen(in))
    return ERR_CODE_OK;
  int status = ERR_CODE_NOT_FOUND;
  char *builtin_str = strncpy_slice(in, 0, next_token_index(in, 0, ' '));

  for (uint32_t i = 0; i < NUM_BUILTINS; i++) {
#if defined(_SH_DEBUG_CMDLINE)
    sh_debugln(DEBUG_CALL_INFO, "sh_strncmp('%s', '%s', %d)", builtin_str,
               builtin_lut[i].str, strlen(builtin_str));
#endif
    if (sh_strncmp(builtin_str, builtin_lut[i].str,
                   strlen(builtin_lut[i].str))) {
      builtin_fn_t fn = builtin_lut[i].fn;
      if (fn == NULL) {
        sh_errorln(
            DEBUG_CALL_INFO,
            "builtin_fn_lut returned nil function pointer for input: '%s'",
            builtin_str);
        status = ERR_CODE_NOT_FOUND;
        break;
      }

#if defined(_SH_DEBUG_CMDLINE)
      sh_debugln(DEBUG_CALL_INFO, "running builtin: %s:0x%p", builtin_str,
                 (uint32_t *)fn);
#endif
      status = fn(0, NULL);
      break;
    }
  }
  return status;
}
