#include <shell.h>
#include <sync/atomic.h>

extern uint32_t asm_get_atomic_uint32(sh_atomic_uint32_t *p);
extern uint32_t asm_set_atomic_uint32(sh_atomic_uint32_t *p, uint32_t new);

uint32_t sh_get_atomic_uint32(sh_atomic_uint32_t *p) {
  uint32_t v = asm_get_atomic_uint32(p);
  return v;
}

uint32_t sh_set_atomic_uint32(sh_atomic_uint32_t *p, uint32_t new) {
  uint32_t prev = asm_set_atomic_uint32(p, new);
  return prev;
}

uint8_t asm_get_atomic_uint8(sh_atomic_uint8_t *p);
uint8_t asm_set_atomic_uint8(sh_atomic_uint8_t *p, uint8_t new);
