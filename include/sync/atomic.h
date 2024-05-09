#ifndef SH_ATOMIC_H
#define SH_ATOMIC_H

#include <stdint.h>

typedef volatile uint32_t sh_atomic_uint32_t;
typedef volatile uint8_t sh_atomic_uint8_t;

extern uint32_t asm_get_atomic_uint32(sh_atomic_uint32_t *p);
extern uint32_t asm_set_atomic_uint32(sh_atomic_uint32_t *p, uint32_t new);

extern uint8_t asm_get_atomic_uint8(sh_atomic_uint8_t *p);
extern uint8_t asm_set_atomic_uint8(sh_atomic_uint8_t *p, uint8_t new);

uint32_t sh_get_atomic_uint32(sh_atomic_uint32_t *p);
uint32_t sh_set_atomic_uint32(sh_atomic_uint32_t *p, uint32_t new);

#endif
