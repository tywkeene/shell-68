#include <stdbool.h>

#include <sync/atomic.h>
#include <sync/once.h>

void sh_do_once(sh_once_t *once, sh_once_fn_t fn) {
  if (sh_once_should_do(once)) {
    fn();
    sh_once_done(once);
  }
}

static bool sh_once_should_do(sh_once_t *once) {
  return (sh_get_atomic_uint32(once) == 0);
}

static void sh_once_done(sh_once_t *once) {
  _do_without_interrupt(sh_set_atomic_uint32(once, 1));
}
