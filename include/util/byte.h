#ifndef SH_UTIL_BYTE_H
#define SH_UTIL_BYTE_H

#include <stdint.h>

#define DWORD_BYTE1_MASK 0xff000000
#define DWORD_BYTE2_MASK 0x00ff0000
#define DWORD_BYTE3_MASK 0x0000ff00
#define DWORD_BYTE4_MASK 0x000000ff

uint8_t get_dword_byte1(uint32_t *val);
uint8_t get_dword_byte2(uint32_t *val);
uint8_t get_dword_byte3(uint32_t *val);
uint8_t get_dword_byte4(uint32_t *val);

void set_dword_byte1(uint32_t *val, uint8_t byte);
void set_dword_byte2(uint32_t *val, uint8_t byte);
void set_dword_byte3(uint32_t *val, uint8_t byte);
void set_dword_byte4(uint32_t *val, uint8_t byte);

#endif
