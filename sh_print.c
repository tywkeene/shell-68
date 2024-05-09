#include <stdnoreturn.h>

#include <printf.h>
#include <sh/api.h>
#include <sh/debug.h>
#include <sh/print.h>
#include <shell.h>
#include <sync/mutex.h>
#include <sys/status_dword.h>
#include <util/misc.h>

sh_mutex_t output_lock;

void sh_printf(const char *fmt, ...) {
  sh_mutex_acquire(&output_lock);

  printf("[SYS]: ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  sh_mutex_release(&output_lock);
  return;
}

void sh_println(const char *fmt, ...) {
  sh_mutex_acquire(&output_lock);

  printf("[SYS]: ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  sh_newline();

  sh_mutex_release(&output_lock);
  return;
}

void sh_errorf(FN_CALL_ARGS, const char *fmt, ...) {
  sh_mutex_acquire(&output_lock);

  printf("[ERROR]: ");
  if (line != -1)
    printf("[%s:%d %s]: ", file, line, fn);
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  sh_mutex_release(&output_lock);
  return;
}

void sh_errorln(FN_CALL_ARGS, const char *fmt, ...) {
  sh_mutex_acquire(&output_lock);

  printf("[ERROR]: ");
  if (line != -1)
    printf("[%s:%d %s]: ", file, line, fn);
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  sh_newline();

  sh_mutex_release(&output_lock);
  return;
}

void sh_debugf(FN_CALL_ARGS, const char *fmt, ...) {
#if !defined(_SH_ANY_DEBUG)
  _arg_not_used(file);
  _arg_not_used(line);
  _arg_not_used(fn);
  _arg_not_used(fmt);
  return;
#else
#pragma message "sh_debugf() enabled"
#endif

  printf("[DEBUG]: ");
  if (line != -1)
    printf("[%s:%d %s]: ", file, line, fn);
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  return;
}

void sh_debugln(FN_CALL_ARGS, const char *fmt, ...) {
#if !defined(_SH_ANY_DEBUG)
  _arg_not_used(file);
  _arg_not_used(line);
  _arg_not_used(fn);
  _arg_not_used(fmt);
  return;
#else
#pragma message "sh_debugln() enabled"
#endif

  printf("[DEBUG]: ");
  if (line != -1)
    printf("[%s:%d %s]: ", file, line, fn);
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  sh_newline();

  return;
}

noreturn void sh_panic(FN_CALL_ARGS, const char *fmt, ...) {
  printf("-----!!!! PANIC !!!!-----\r\n");
  sys_print_status_dword();
  printf("[PANIC]: ");

  if (line != -1)
    printf("[%s:%d %s]: ", file, line, fn);

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  printf("\r\n-----!!!! PANIC !!!!-----\r\n");
  sh_get_api()->sys_halt();
  __builtin_unreachable();
}
