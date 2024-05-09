#include <stdio.h>
#include <stdnoreturn.h>

#include <io/std.h>
#include <io/term.h>
#include <sh/api.h>
#include <sh/heap.h>
#include <sh/print.h>
#include <sh/status_led.h>
#include <sys/disk.h>
#include <sys/status_dword.h>
#include <sys/task.h>

extern void *_start;
extern void *_end;

static sh_api_t sh;

extern void sh_status_led_task_loop(void);

SH_API_PUBLIC sh_api_t *sh_get_api(void) {
  sh_api_t *p = &sh;
  if (p != NULL)
    return &sh;
  sh_panic(DEBUG_CALL_INFO, "shell api null");
  __builtin_unreachable();
}

static TASK_INIT_FN _init(void) {
  sh.sys_disable_interrupts();
  sh_task_schedule(sh.led_task);
  sh_task_schedule(sh.shell_task);
  sh.sys_enable_interrupts();
}

SH_API_PRIVATE void init_tasking(void) {
  sh.tasks = sh_initialize_task_list();
  sh.init_task = sh_create_task("init", SYS_STACK_SIZE, TASK_PRIORITY_3, _init);
  sh.led_task = sh_create_task("status_led", SYS_STACK_SIZE, TASK_PRIORITY_3,
                               init_status_led);
  sh.shell_task = sh_create_task("shell", USER_STACK_SIZE, TASK_PRIORITY_3,
                                 sh_main_shell_loop);
  sys_set_status(SYS_K_TASK_MASK);
}

SH_API_PRIVATE void init_pmm() {
  uint32_t free_start = (uint32_t)&_end;
  uint32_t free_aligned = free_start & 0xfffff000;
  sh_println("code start addr: 0x%#08x", (uint32_t)&_start);
  sh_println("       end addr: 0x%#08x", (uint32_t)&_end);
  sh_println("           size: %u bytes", (uint32_t)(&_end - &_start));

  if (free_aligned < free_start)
    free_aligned = free_aligned + 1024;
  if (free_aligned >= 0xefc00)
    sh_panic(DEBUG_CALL_INFO, "pmm out of memory");

  sh.k_api->mem_free(free_aligned, 0xf0000);
  sys_set_status(SYS_K_PMM_MASK);
}

SH_API_PRIVATE void k_disable_interrupts(void) {
  sys_unset_status(SYS_K_INTERRUPTS_MASK);
  sh.k_api->disable_interrupts();
}

SH_API_PRIVATE void k_enable_interrupts(void) {
  sh.k_api->enable_interrupts();
  sys_set_status(SYS_K_INTERRUPTS_MASK);
}

SH_API_PRIVATE void init_k_api(void) {
  ensure_kernel_api();
  sh.k_api = get_kernel_api();
  if (sh.k_api == NULL) {
    sh_panic(DEBUG_CALL_INFO, "failed to get kernel api handle");
  }
  sys_set_status(SYS_K_API_MASK);
}

SH_API_PRIVATE noreturn void default_halt(void) {
  sh_println("SYSTEM HALTING");
  k_disable_interrupts();
  mcHalt();
  __builtin_unreachable();
}

SH_API_PRIVATE void init_sys(void) {
  sh.sys_disable_interrupts = k_disable_interrupts;
  sh.sys_enable_interrupts = k_enable_interrupts;
  sh.sys_halt = default_halt;
}

SH_API_PUBLIC noreturn void sys_start() {
  sh_task_start(sh.init_task);
  __builtin_unreachable();
}

SH_API_PRIVATE void sh_io_std_init(void) {
  sh.gets = std_gets;
  sh.puts = std_puts;
  sh.putch = std_putch;
  sh.getch = std_getch;
  sh.put_newline = std_newline;
  sh.set_cursor_xy = std_set_cursor_xy;
}

SH_API_PRIVATE void sh_io_vt_init(void) {
  init_vt_lib();
  sys_set_status(SYS_SH_TERM_MASK);

  sh.gets = term_gets;
  sh.puts = term_puts;
  sh.putch = term_putch;
  sh.getch = term_getch;
  sh.put_newline = term_newline;
  sh.set_cursor_xy = term_set_cursor_xy;
}

SH_API_PUBLIC void sh_initialize_api(term_type_t term) {
  sys_init_status();

  switch (term) {
  case TERM_STD:
    sh_io_std_init();
    break;
  case TERM_VT:
    sh_io_vt_init();
    break;
  }
  init_message("k/api", init_k_api);
  init_message("k/pmm", init_pmm);
  init_message("sh/sys", init_sys);
  init_message("sh/heap", sh_init_heap);
  init_message("sys/disk", sys_init_disk);
  sys_set_status(SYS_SH_API_MASK);

  sh.sys_enable_interrupts();
  init_tasking();
  sys_print_status_dword();
}

SH_API_PUBLIC char sh_getch(void) { return sh.getch(); }
SH_API_PUBLIC void sh_putch(const char c) { sh.putch(c); };
SH_API_PUBLIC char *sh_gets(char *buffer, size_t n) {
  return sh.gets(buffer, n);
};
SH_API_PUBLIC void sh_puts(const char *out) { sh.puts(out); };
SH_API_PUBLIC void sh_newline(void) { sh.put_newline(); };

SH_API_PUBLIC void sh_prompt(const char *prompt, char *buffer, size_t n) {
  sh.puts(prompt);
  buffer = sh.gets(buffer, n);
};
