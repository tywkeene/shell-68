#ifndef SH_STATUS_LED_H
#define SH_STATUS_LED_H

#include <gpio_spi.h>
#include <shell.h>
#include <stdint.h>

#define SYS_FLAG_WORD 0x40A

#define SIGNAL_LED_SET_STATE 0x00000001

typedef uint32_t led_status_t;

enum led_status_t { LED_OFF = 0, LED_ON, LED_BLINK, LED_EXIT, NUM_LED_STATUS };

#define INIT_STATUS_LED() *((volatile uint16_t *)SYS_FLAG_WORD) &= ~0x0300

void sh_status_led_toggle(void);
void sh_status_led_on(void);
void sh_status_led_off(void);
void init_status_led();

#endif
