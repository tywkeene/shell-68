#include <stdint.h>
#include <stdio.h>

#include <util/byte.h>

uint8_t get_dword_byte1(uint32_t *val) {
  return (*val & DWORD_BYTE1_MASK) >> 24;
}

uint8_t get_dword_byte2(uint32_t *val) {
  return (*val & DWORD_BYTE2_MASK) >> 16;
}

uint8_t get_dword_byte3(uint32_t *val) {
  return (*val & DWORD_BYTE3_MASK) >> 8;
}

uint8_t get_dword_byte4(uint32_t *val) { return (*val & DWORD_BYTE4_MASK); }

void set_dword_byte1(uint32_t *val, uint8_t byte) {
  *val = (*val & ~DWORD_BYTE1_MASK) | ((uint32_t)byte << 24);
}

void set_dword_byte2(uint32_t *val, uint8_t byte) {
  *val = (*val & ~DWORD_BYTE2_MASK) | ((uint32_t)byte << 16);
}

void set_dword_byte3(uint32_t *val, uint8_t byte) {
  *val = (*val & ~DWORD_BYTE3_MASK) | ((uint32_t)byte << 8);
}

void set_dword_byte4(uint32_t *val, uint8_t byte) {
  *val = (*val & ~DWORD_BYTE4_MASK) | byte;
}
