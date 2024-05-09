#ifndef SH_UTIL_STRING_H
#define SH_UTIL_STRING_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

bool isascii(int in);
uint16_t ascii_or_dot(uint16_t val);

uint32_t next_token_index(char *in, size_t off, char token);
char *strncpy_slice(const char *restrict src, size_t off, size_t n);
uint32_t count_tokens(const char *restrict in, char token);
bool sh_strncmp(const char *cmp, const char *buf, size_t n);
const char *parse_err_str(int err);
void strip_newline(char *str);

#endif
