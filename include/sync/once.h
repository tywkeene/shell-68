#ifndef SH_SYNC_ONCE_H
#define SH_SYNC_ONCE_H

#include <sync/atomic.h>
#include <stdbool.h>
#include <shell.h>

#define SH_ONCE_INIT {0}

typedef sh_atomic_uint32_t sh_once_t;

typedef void (*sh_once_fn_t)(void);

static bool sh_once_should_do(sh_once_t *once);
static void sh_once_done(sh_once_t *once);

#endif
