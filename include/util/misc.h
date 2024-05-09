#ifndef SH_UTIL_MISC_H
#define SH_UTIL_MISC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define _arg_not_used(x) (void)x;

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

void sync_copy(void *dst, void *src, size_t size);
void sh_prompt_input(const char *prompt, char *buffer, size_t max);
void press_the_any_key(void);
void sh_hex_dump(uint32_t *start, uint32_t *end);

void sh_sleep_sec(uint32_t mul);
void sh_sleep_500ms(void);
void sh_sleep_ms(uint32_t n);

#endif
