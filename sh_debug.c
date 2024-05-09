#include <stdint.h>
#include <stdio.h>

#include <sh/debug.h>
#include <util/misc.h>

extern uint32_t *asm_get_sp(void);
extern uint32_t *asm_get_fp(void);

__attribute__((noinline)) void sh_debug_stacktrace(uint32_t count) {
  _arg_not_used(count);

  uint32_t *sp = asm_get_sp();
  uint32_t *fp = asm_get_fp();

  sh_println("sp: 0x%p", sp);
  sh_println("fp: 0x%p", fp);
}
