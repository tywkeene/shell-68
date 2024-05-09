#include <kernelapi.h>
#include <printf.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <kmain.h>
#include <sh/api.h>
#include <sh/debug.h>
#include <sh/heap.h>
#include <sh/print.h>
#include <shell.h>
#include <sys/status_dword.h>
#include <util/misc.h>

uint32_t bytes_free = _SH_HEAP_SIZE;

static sh_heap_t *sh_heap;

void sh_heap_debug_info(void) {
  uint16_t index = sh_heap->header.index;
  sh_block_header_t *first = &sh_heap->blocks[0];
  sh_block_header_t *last = &sh_heap->blocks[index];
  uint16_t first_addr = get_rm16(sh_heap->start_addr);

  sh_println("----------------------------|    heap info    "
             "|----------------------------");
  sh_println("      bytes free: %u/%u", bytes_free, _SH_HEAP_SIZE);
  sh_println(" blocks allocated: %u/%u", index, _SH_MAX_BLOCKS);
  sh_println("");
  sh_println(" heap start addr: 0x%p", (uint32_t)sh_heap->start_addr);
  sh_println(" address space: 0x%04x first addr: 0x%04x",
             (uint16_t)sh_heap->header.addr_space, (uint16_t)first_addr);
  sh_println("           first: %s", sh_block_header_str(first));
  sh_println("            last: %s", sh_block_header_str(last));
  sh_println("-----------------------------------------------------------------"
             "----------");
}

const char *sh_block_header_str(sh_block_header_t *blk) {
  uint32_t real_addr = blk_real_addr(sh_heap->header.addr_space, blk->addr);
  static char str[128];
  sprintf(str, "blk_addr: 0x%04x real_addr: 0x%08x size: %u bytes",
          (uint16_t)(blk->addr), (uint32_t)real_addr, (uint16_t)blk->size);
  return str;
}

const char *sh_heap_header_str(void) {
  uint16_t addr_space = sh_heap->header.addr_space;
  uint16_t index = sh_heap->header.index;
  static char str[128];
  sprintf(str, "addr_space: 0x%04x start_addr: 0x%p index: %u", addr_space,
          sh_heap->start_addr, index);
  return str;
}

void sh_init_heap(void) {
  sh_api_t *sh = sh_get_api();
  sh_heap = (sh_heap_t *)sh->k_api->mem_alloc(sizeof(sh_heap_t *));
  memset(sh_heap, 0, sizeof(sh_heap_t));

  sh_heap->blocks = (sh_block_header_t *)sh->k_api->mem_alloc(
      sizeof(sh_block_header_t) * _SH_MAX_BLOCKS);
  memset(sh_heap->blocks, 0, sizeof(sh_block_header_t) * _SH_MAX_BLOCKS);

  sh_heap->start_addr = (uint32_t *)sh->k_api->mem_alloc(_SH_HEAP_SIZE);
  memset(&(*sh_heap->start_addr), 0, _SH_HEAP_SIZE);

  sh_heap->header.addr_space = get_lm16(((uint32_t *)(&sh_heap->start_addr)));
  sh_heap->header.index = 0;

  sh_heap->blocks[0].addr = get_rm16(sh_heap->start_addr);
  sh_heap->blocks[0].size = 0;

  bytes_free = _SH_HEAP_SIZE;
  sys_set_status(SYS_SH_HEAP_MASK);
}

sh_block_header_t *sh_alloc_block(uint16_t alloc_size) {
  if (sh_heap->header.index >= _SH_MAX_BLOCKS) {
    sh_panic(DEBUG_CALL_INFO,
             "out of memory -- sh_alloc_free_block has no free blocks");
  }
  if (bytes_free <= alloc_size) {
    sh_panic(DEBUG_CALL_INFO,
             "out of memory -- cannot allocate %u bytes with %u bytes free",
             alloc_size, bytes_free);
  }

  uint16_t prev_idx = sh_heap->header.index;
  uint16_t alloc_idx = sh_heap->header.index + 1;
  sh_block_header_t *prev = &sh_heap->blocks[prev_idx];
  sh_block_header_t *alloc = &sh_heap->blocks[alloc_idx];

  alloc->addr = (prev->addr + prev->size);
  alloc->size = alloc_size;

  uint16_t head_idx = alloc_idx + 1;
  sh_heap->blocks[head_idx].addr = (alloc->addr + alloc->size);
  sh_heap->blocks[head_idx].size = 0;

#if defined(_SH_DEBUG_MALLOC)
  sh_println(
      "------------------------------------------------------------------");
  sh_println("       prev block %u: %s", prev_idx, sh_block_header_str(prev));
  sh_println("     alloc block %u: %s", alloc_idx, sh_block_header_str(alloc));
  sh_println("      head block %u: %s", head_idx,
             sh_block_header_str(&sh_heap->blocks[head_idx]));
  sh_println(
      "------------------------------------------------------------------");
#endif
  sh_heap->header.index++;
  bytes_free -= alloc_size;
  return &sh_heap->blocks[alloc_idx];
}

void sh_debug_mem_blocks_info(void) {
  if (sh_heap->header.index > 0) {
    sh_println("----------------------------| allocated blocks "
               "|----------------------------");
    for (uint16_t i = 0; i < sh_heap->header.index; i++) {
      sh_println("%u: %s", i, sh_block_header_str(&sh_heap->blocks[i]));
    }
    sh_println("---------------------------------------------------------------"
               "-------------");
  } else {
    sh_println("no blocks allocated");
  }
}

void sh_free(void *p) {
  (void)p;
  sh_panic(DEBUG_CALL_INFO, "sh_free() not implemented");
}

void *sh_malloc(uint16_t size) {
  sh_block_header_t *alloc = sh_alloc_block(size);
  uint32_t real_addr = blk_real_addr(sh_heap->header.addr_space, alloc->addr);
#if defined(_SH_DEBUG_MALLOC)
  sh_debugln(DEBUG_CALL_INFO, "malloc(%d) -> real_addr 0x%p", size, real_addr);
#endif
  return (void *)real_addr;
}

void *sh_calloc(uint32_t n, uint16_t size) {
  uint16_t alloc_size = (n * size);
  uint32_t *p = sh_malloc(alloc_size);
  memset(p, 0, alloc_size);
#if defined(_SH_DEBUG_MALLOC)
  sh_debugln(DEBUG_CALL_INFO, "calloc(%d,%d) -> real_addr 0x%p", n, size, p);
#endif
  return p;
}
