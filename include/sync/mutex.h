#ifndef SH_MUTEX_H
#define SH_MUTEX_H

#include <sync/atomic.h>

typedef struct {
  sh_atomic_uint32_t state;
} sh_mutex_t;

#define SH_MUTEX_FREE 0
#define SH_MUTEX_HELD 1

#define SH_INIT_MUTEX(m) m.state = SH_MUTEX_FREE

void sh_mutex_acquire(sh_mutex_t *m);
void sh_mutex_release(sh_mutex_t *m);

void sh_debug_mutex_state(sh_mutex_t *m);

#endif
