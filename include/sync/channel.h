#ifndef SH_CHANNEL_H
#define SH_CHANNEL_H

#include <stdbool.h>
#include <stdlib.h>

#include <util/misc.h>
#include <sync/atomic.h>
#include <sync/mutex.h>

#define SH_CHAN_FULL 1
#define SH_CHAN_EMPTY 2
#define SH_CHAN_CLOSE 3

#define SH_CHAN_WAIT true
#define SH_CHAN_NOWAIT false

#define CHAN_MAX_NAME 32

typedef struct {
  char name[CHAN_MAX_NAME];
  sh_mutex_t d_lock;
  void *data;

  sh_atomic_uint32_t status;

  size_t size;

  bool wait_read;
  bool wait_write;
} sh_channel_t;

sh_channel_t *sh_init_channel(const char * restrict name, size_t size, bool wait_read, bool wait_write);
int sh_channel_write(sh_channel_t *ch, void *input);
void *sh_channel_read(sh_channel_t *ch);
void sh_channel_set_status(sh_channel_t *ch, uint32_t new);
uint32_t sh_channel_get_status(sh_channel_t *ch);
int sh_channel_close(sh_channel_t *ch);

#endif
