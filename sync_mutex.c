#include <sh/debug.h>
#include <sh/print.h>
#include <sync/atomic.h>
#include <sync/mutex.h>
#include <util/misc.h>

static void _wait_set(sh_mutex_t *m, uint32_t new) {
  uint32_t val;
  while ((val = sh_get_atomic_uint32(&m->state)) != SH_MUTEX_FREE)
    mcBusywait(50000);
  sh_set_atomic_uint32(&m->state, new);
}

void sh_mutex_acquire(sh_mutex_t *m) {
#if defined(_SH_DEBUG_MUTEX)
  sh_debugln(DEBUG_CALL_INFO, "acquiring");
  sh_debug_mutex_state(m);
#endif
  _wait_set(m, SH_MUTEX_HELD);
}

void sh_mutex_release(sh_mutex_t *m) {
#if defined(_SH_DEBUG_MUTEX)
  sh_debugln(DEBUG_CALL_INFO, "releasing");
  sh_debug_mutex_state(m);
#endif
  sh_set_atomic_uint32(&m->state, SH_MUTEX_FREE);
}

void sh_debug_mutex_state(sh_mutex_t *m) {
#if defined(_SH_DEBUG_MUTEX)
  uint32_t l = sh_get_atomic_uint32(&m->state);
  sh_debugln(DEBUG_CALL_INFO, "[locked=%u]", l);
  return;
#endif
  _arg_not_used(m);
}
