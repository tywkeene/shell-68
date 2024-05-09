#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sh/api.h>
#include <sh/errcode.h>
#include <sh/print.h>
#include <sync/channel.h>
#include <sys/status_dword.h>

#define _chan_wait_status(_ch, _status)                                        \
  do {                                                                         \
    mcBusywait(50000);                                                         \
  } while (sh_channel_get_status((sh_channel_t *)_ch) != (uint32_t)_status)

#if defined(_SH_DEBUG_CHAN)
static void chan_debugfn(FN_CALL_ARGS, sh_channel_t *ch) {
  sh_debugln(line, file, fn, "[name=%s size=%u, wait_read=%d, wait_write=%d)",
             ch->name, ch->size, ch->wait_read, ch->wait_write);
}
#else
static void chan_debugfn(FN_CALL_ARGS, sh_channel_t *ch) {
  _arg_not_used(fn);
  _arg_not_used(line);
  _arg_not_used(file);
  _arg_not_used(ch);
}
#endif

sh_channel_t *sh_init_channel(const char *restrict name, size_t size,
                              bool wait_read, bool wait_write) {
  sh_channel_t *ch = (sh_channel_t *)sh_malloc(sizeof(sh_channel_t *));
  strncpy(ch->name, name, CHAN_MAX_NAME);

  ch->size = size;
  ch->wait_read = wait_read;
  ch->wait_write = wait_write;
  ch->data = (void *)sh_calloc(1, size);

  ch->d_lock.state = SH_MUTEX_FREE;
  sh_set_atomic_uint32(&ch->status, SH_CHAN_EMPTY);

  chan_debugfn(DEBUG_CALL_INFO, ch);
  return ch;
}

static inline int _chan_wait_write(sh_channel_t *ch, void *input) {
  _chan_wait_status(ch, SH_CHAN_EMPTY);
  chan_debugfn(DEBUG_CALL_INFO, ch);

  sh_mutex_acquire(&ch->d_lock);
  memset(ch->data, 0, ch->size);
  sync_copy(ch->data, input, ch->size);
  sh_mutex_release(&ch->d_lock);

  sh_channel_set_status(ch, SH_CHAN_FULL);
  return ERR_CODE_CHAN_OK;
}

static inline void *_chan_wait_read(sh_channel_t *ch) {
  _chan_wait_status(ch, SH_CHAN_FULL);
  chan_debugfn(DEBUG_CALL_INFO, ch);

  void *p = (void *)sh_malloc(ch->size);
  sh_mutex_acquire(&ch->d_lock);
  sync_copy(p, ch->data, ch->size);
  memset(ch->data, 0, ch->size);
  sh_mutex_release(&ch->d_lock);

  sh_channel_set_status(ch, SH_CHAN_EMPTY);
  return p;
}

int sh_channel_write(sh_channel_t *ch, void *input) {
  uint32_t err = ERR_CODE_CHAN_OK;
  uint32_t ch_status = sh_channel_get_status(ch);

  switch (ch_status) {
  case SH_CHAN_CLOSE:
    sh_errorln(DEBUG_CALL_INFO, "[%s]: attempted write on closed channel",
               ch->name);
    err = ERR_CODE_CHAN_CLOSED;
    break;
  case SH_CHAN_FULL:
    if (ch->wait_write == SH_CHAN_NOWAIT) {
      break;
    } else {
      err = _chan_wait_write(ch, input);
      break;
    }
  case SH_CHAN_EMPTY:
    err = _chan_wait_write(ch, input);
    break;
  default:
    sh_errorln(DEBUG_CALL_INFO, "channel in unknown state: %u", ch_status);
    sh_channel_set_status(ch, SH_CHAN_EMPTY);
    err = ERR_CODE_ERROR;
    break;
  }
  return err;
}

void *sh_channel_read(sh_channel_t *ch) {
  void *p = NULL;
  uint32_t ch_status = sh_channel_get_status(ch);

  switch (ch_status) {
  case SH_CHAN_CLOSE:
    sh_errorln(DEBUG_CALL_INFO, "[chan: %s]: attempted read on closed channel",
               ch->name);
    break;
  case SH_CHAN_EMPTY:
    if (ch->wait_read == SH_CHAN_NOWAIT) {
      break;
    } else {
      p = _chan_wait_read(ch);
      break;
    }
  case SH_CHAN_FULL:
    p = _chan_wait_read(ch);
    break;
  default:
    sh_errorln(DEBUG_CALL_INFO, "channel in unknown state: %u", ch_status);
    sh_channel_set_status(ch, SH_CHAN_FULL);
    break;
  }
  return p;
}

void sh_channel_set_status(sh_channel_t *ch, uint32_t new) {
  sh_set_atomic_uint32(&ch->status, new);
}

uint32_t sh_channel_get_status(sh_channel_t *ch) {
  return sh_get_atomic_uint32(&ch->status);
}

int sh_channel_close(sh_channel_t *ch) {
  chan_debugfn(DEBUG_CALL_INFO, ch);
  if (sh_channel_get_status(ch) == SH_CHAN_CLOSE)
    return ERR_CODE_CHAN_CLOSED;

  sh_channel_set_status(ch, SH_CHAN_CLOSE);
  ch->data = NULL;
  return ERR_CODE_CHAN_OK;
}
