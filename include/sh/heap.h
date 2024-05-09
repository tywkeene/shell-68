#ifndef SH_MALLOC_H
#define SH_MALLOC_H

#include <stddef.h>
#include <stdint.h>

// 64kb
#define _SH_HEAP_SIZE 65535
#define _SH_BLOCK_DIV 64
#define _SH_MAX_BLOCKS (_SH_HEAP_SIZE / _SH_BLOCK_DIV)

#define set_rm16(_v, _r) _v |= (uint16_t)_r
#define set_lm16(_v, _l) _v |= (uint32_t)((_l << 16) & 0xFFFF0000)

#define get_lm16(_x) (uint16_t)(((uint32_t)_x & 0xFFFF0000) >> 16)
#define get_rm16(_x) (uint16_t)((uint32_t)_x & 0x0000FFFF)

#define construct_32bit(_l, _r) (uint32_t)((_l << 16) | _r)
#define blk_real_addr(_space, _blk) construct_32bit(_space, _blk)

// ------------------------------ //

// sh_heap_header_t (uint32_t)
//   space   index
// [ 00 00 | 00 00 ] 4 bytes

// sh_block_header_t (uint32_t)
//   addr    size
// [ 00 00 | 00 00 ] 4 bytes

// sh_heap_header_t->sh_block_header_t[uint16_t]
// [ 00 00 | 00 00 ] 4 bytes
// [ 00 00 | 00 00 ] 4 bytes
// [ 00 00 | 00 00 ] 4 bytes
// ...n (uint16_t max)

typedef struct {
  uint16_t size : 16;
  uint16_t addr : 16;
} sh_block_header_t;

typedef struct {
  uint16_t index : 16;
  uint16_t addr_space : 16;
} sh_heap_header_t;

typedef struct {
  sh_heap_header_t header;
  sh_block_header_t *blocks;
  uint32_t *start_addr;
} __attribute__((aligned(2))) sh_heap_t;

void sh_heap_debug_info(void);

void sh_init_heap(void);

void *sh_malloc(uint16_t size);
void *sh_calloc(uint32_t n, uint16_t size);
void sh_free(void *p);

const char *sh_heap_header_str(void);
const char *sh_block_header_str(sh_block_header_t *blk);

void sh_debug_mem_blocks_info(void);

sh_block_header_t *sh_alloc_block(uint16_t alloc_size);

#endif
