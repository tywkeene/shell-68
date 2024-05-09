#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sh/errcode.h>
#include <sh/heap.h>
#include <sh/print.h>
#include <sys/status_dword.h>
#include <sys/task.h>
#include <util/misc.h>

#define INPUT_BUFFER_SIZE 64

extern uint32_t bytes_free;

static const char *make_prompt(int status) {
  static char buf[32];
  sprintf(buf, "[%d bytes free][status: %s]\r\n$ ", bytes_free,
          errcode_str(status));
  return buf;
}

TASK_INIT_FN sh_main_shell_loop(void) {
  sh_println("system ready...");
  char *buffer = sh_calloc(INPUT_BUFFER_SIZE, sizeof(char));
  int status = ERR_CODE_OK;
  while (true) {
    sh_prompt_input(make_prompt(status), buffer, INPUT_BUFFER_SIZE);
    status = execute_builtin(buffer);
    switch (status) {
    case ERR_CODE_OK:
      continue;
    case ERR_CODE_NOT_FOUND:
      continue;
    case ERR_CODE_ERROR:
      continue;
    case ERR_CODE_PANIC:
      sh_panic(DEBUG_CALL_INFO, "shell returned panic status: buffer: '%s'\r\n",
               buffer);
    }
  }
}
