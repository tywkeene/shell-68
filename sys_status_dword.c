#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sync/atomic.h>
#include <sys/status_dword.h>
#include <util/misc.h>

status_dword_t sys_status;

static const uint32_t sys_status_masks[] = {
    SYS_K_API_MASK,   SYS_K_PMM_MASK,   SYS_K_INTERRUPTS_MASK, SYS_K_TASK_MASK,
    SYS_SH_TERM_MASK, SYS_SH_HEAP_MASK, SYS_SH_LED_MASK,       SYS_SH_DISK_MASK,
};

void sys_init_status(void) { sh_set_atomic_uint32(&sys_status, 0); }

void sys_unset_status(uint32_t mask) {
  uint32_t val = sh_get_atomic_uint32(&sys_status);
  val = (val | !mask);
  sh_set_atomic_uint32(&sys_status, val);
}

void sys_set_status(uint32_t mask) {
  uint32_t val = sh_get_atomic_uint32(&sys_status);
  val = (val | mask);
  sh_set_atomic_uint32(&sys_status, val);
}

bool sys_get_status(uint32_t mask) {
  return (sh_get_atomic_uint32(&sys_status) & mask);
}

void sys_print_status_dword(void) {
  static const struct {
    char *name;
  } status_name_lut[SYS_STATUS_MASK_MAX] = {
      [SYS_K_API_MASK] = {"K_API"},        [SYS_K_PMM_MASK] = {"K_PMM"},
      [SYS_K_INTERRUPTS_MASK] = {"K_INT"}, [SYS_K_TASK_MASK] = {"SH_TASK"},
      [SYS_SH_TERM_MASK] = {"SH_TERM"},    [SYS_SH_HEAP_MASK] = {"SH_HEAP"},
      [SYS_SH_LED_MASK] = {"SH_LED_CTRL"}, [SYS_SH_DISK_MASK] = {"SYS_DISK"},
  };

  char tmp[32];
  char out[128];
  memset(out, 0, 128);
  memset(tmp, 0, 32);
  for (size_t i = 0; i < sizeof_array(sys_status_masks); i++) {
    uint32_t mask = sys_status_masks[i];
    bool val = sys_get_status(mask);
    char *str = status_name_lut[mask].name;
    sprintf(tmp, " %s=%d ", str, val);
    strncat(out, tmp, strlen(tmp));
    memset(tmp, 0, strlen(tmp));
  }
  memset(&out[strlen(out)], 0, 128 - strlen(out));
  sh_println("status: %s", out);
}
