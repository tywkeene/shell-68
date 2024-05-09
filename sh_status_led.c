#include <stdint.h>

#include <sh/api.h>
#include <sh/debug.h>
#include <sh/print.h>
#include <sh/status_led.h>
#include <shell.h>
#include <sync/atomic.h>
#include <sync/channel.h>
#include <sys/status_dword.h>
#include <sys/task.h>
#include <util/misc.h>

// HI/LO refer to physical pin state, these ports are active
// low, so they appear reversed here...
static volatile uint8_t *DUART_OUTLOPORT = (uint8_t *)DUART_W_OPR_SETCMD;
static volatile uint8_t *DUART_OUTHIPORT = (uint8_t *)DUART_W_OPR_RESETCMD;

static const uint8_t SH_STATUS_LED = 0x08; // Red on-board LED

sh_atomic_uint32_t led_initialized = 0;
sh_atomic_uint32_t led_current_state = 0;
sh_atomic_uint32_t led_set_state = 0;

static const led_status_t toggle_lut[NUM_LED_STATUS] = {
    [LED_ON] = LED_OFF,
    [LED_OFF] = LED_ON,
    [LED_BLINK] = LED_OFF,
    [LED_EXIT] = LED_EXIT,
};

void sh_status_led_toggle(void) {
  uint32_t current_state = sh_get_atomic_uint32(&led_current_state);
  sh_set_atomic_uint32(&led_set_state, toggle_lut[current_state]);
  sh_task_signal(sh_get_api()->led_task, T_SIGNAL_WAKE);
}

void sh_status_led_on(void) {
  if (!sh_get_atomic_uint32(&led_initialized)) {
    sh_errorln(DEBUG_CALL_INFO, "system status led not initialized");
    return;
  }

  sh_set_atomic_uint32(&led_set_state, LED_ON);
  sh_task_signal(sh_get_api()->led_task, T_SIGNAL_WAKE);
}

void sh_status_led_off(void) {
  if (!sh_get_atomic_uint32(&led_initialized)) {
    sh_errorln(DEBUG_CALL_INFO, "system status led not initialized");
    return;
  }
  sh_set_atomic_uint32(&led_set_state, LED_OFF);
  sh_task_signal(sh_get_api()->led_task, T_SIGNAL_WAKE);
}

static void _set_led_on(void) {
  (void)(*DUART_OUTLOPORT = SH_STATUS_LED);
  sh_set_atomic_uint32(&led_current_state, LED_ON);
}

static void _set_led_off(void) {
  (void)(*DUART_OUTHIPORT = SH_STATUS_LED);
  sh_set_atomic_uint32(&led_current_state, LED_OFF);
}

void sh_status_led_task_loop(void) {
  sh_set_atomic_uint32(&led_initialized, 1);
  sh_set_atomic_uint32(&led_set_state, LED_ON);

  while (sys_get_status(SYS_SH_LED_MASK)) {
    sh_task_wait();
    led_status_t set = (led_status_t)sh_get_atomic_uint32(&led_set_state);
    switch (set) {
    case LED_OFF:
      _do_without_interrupt(_set_led_off);
      continue;
    case LED_ON:
      _do_without_interrupt(_set_led_on);
      continue;
    case LED_EXIT:
      return;
    default:
      sh_panic(DEBUG_CALL_INFO, "undefined led_status_t: %u", set);
    }
  }
  sys_unset_status(SYS_SH_LED_MASK);
  sh_panic(DEBUG_CALL_INFO, "exiting");
}

TASK_INIT_FN init_status_led(void) {
  sh_api_t *api = sh_get_api();
  api->sys_disable_interrupts();

  sh_println("initializing status led...");
  INIT_STATUS_LED();
  _set_led_on();
  sys_set_status(SYS_SH_LED_MASK);

  api->sys_enable_interrupts();

  sh_status_led_task_loop();
}
