#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <sh/api.h>
#include <util/misc.h>

void sync_copy(void *dst, void *src, size_t size) {
  sh_api_t *api = sh_get_api();
  api->sys_disable_interrupts();
  memcpy(dst, src, size);
  api->sys_enable_interrupts();
}

void sh_prompt_input(const char *prompt, char *buffer, size_t max) {
  sh_puts(prompt);
  memset(buffer, 0, max);
  char *out = sh_gets(buffer, max);
  strncpy(buffer, out, strlen(out));
}

void press_the_any_key(void) {
  sh_println("press the any key to continue...");
  sh_getch();
}

void sh_hex_dump(uint32_t *start, uint32_t *end) {
  sh_println("|  [0x%p]   to    [0x%p]  |", start, end);
  for (uint32_t *p = start; p < end; p++) {
    sh_println("| [0x%p]: %02x %02x %02x %02x ", (void *)p, (uint8_t)(*p >> 24),
               (uint8_t)(*p >> 16), (uint8_t)(*p >> 8), (uint8_t)(*p));
    sh_println("| %c %c %c %c |", ascii_or_dot((uint8_t)(*p >> 24)),
               ascii_or_dot((uint8_t)(*p >> 16)),
               ascii_or_dot((uint8_t)(*p >> 8)), ascii_or_dot((uint8_t)(*p)));
  }
  sh_println("----------------| END |---------------+");
}

void sh_sleep_sec(uint32_t mul) {
  if (mul > 0)
    mcDelaymsec10(10 * mul);
}

void sh_sleep_500ms(void) { sh_sleep_ms(500); }

void sh_sleep_ms(uint32_t n) {
  // about 1000 microseconds in a millisecond
  // *   ~1.80 usec per tick with 10Mhz CPU
  // so about half-ish should be about-ish 1ms
  // or whatever idk lmao
  mcBusywait(n * 500);
}
