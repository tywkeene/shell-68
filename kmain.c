#include <debug_stub.h>
#include <stdnoreturn.h>

#include <sh/api.h>
#include <shell.h>

noreturn void kmain(void) {
  debug_stub();
  sh_initialize_api(TERM_STD);
  sys_start();
  __builtin_unreachable();
}
