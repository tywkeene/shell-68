#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <util/string.h>

bool isascii(int in) {
  if (in >= 0x20 && in <= 0x7E)
    return true;
  return false;
}

uint16_t ascii_or_dot(uint16_t val) {
  if (isascii(val) && val != (char)0x00)
    return val;
  return '.';
}

uint32_t next_token_index(char *in, size_t off, char token) {
  uint32_t i = off;
  if (in[i] == token)
    i++;
  for (; i < strlen(in); i++)
    if (in[i] == token)
      break;
  return i;
}

inline char *strncpy_slice(const char *restrict src, size_t off, size_t n) {
  char *dst = (char *)sh_calloc(n + 1, sizeof(char));
  size_t max = off + n;
  size_t src_idx = off, dst_idx = 0;

  for (; src_idx < max && dst_idx < n; src_idx++, dst_idx++)
    dst[dst_idx] = src[src_idx];
  dst[dst_idx] = 0;
  return dst;
}

inline uint32_t count_tokens(const char *restrict in, char token) {
  uint32_t count = 0;
  for (uint32_t i = 0; i < strlen(in); i++)
    if (in[i] == token)
      count++;
  return count;
}

inline bool sh_strncmp(const char *cmp, const char *buf, size_t n) {
  for (size_t i = 0; i < n; i++)
    if (cmp[i] != buf[i])
      return false;
  return true;
}

inline void strip_newline(char *str) {
  for (uint32_t i = 0; i < strlen(str); i++)
    if (str[i] == '\n')
      str[i] = '\0';
}
