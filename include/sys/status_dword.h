#ifndef SH_STATUS_H
#define SH_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#include <sync/atomic.h>

enum {
  SYS_K_API_MASK = (1u << 0),
  SYS_K_PMM_MASK = (1u << 1),
  SYS_K_INTERRUPTS_MASK = (1u << 2),
  SYS_K_TASK_MASK = (1u << 3),
  SYS_SH_TERM_MASK = (1u << 4),
  SYS_SH_HEAP_MASK = (1u << 5),
  SYS_SH_LED_MASK = (1u << 6),
  SYS_SH_API_MASK = (1u << 7),
  SYS_STATUS_MASK_MAX
};

typedef sh_atomic_uint32_t status_dword_t;

void sys_init_status(void);
void sys_set_status(uint32_t mask);
bool sys_get_status(uint32_t mask);
void sys_print_status_dword(void);

#endif
