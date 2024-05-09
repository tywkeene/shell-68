#include <stdint.h>

#include <sh/errcode.h>

inline const char *errcode_str(uint8_t err) {
  static const struct {
    const char *msg;
  } err_msg_lut[NUM_ERRCODE] = {
      [ERR_CODE_OK] = {"ok"},
      [ERR_CODE_EXIT] = {"user exited"},
      [ERR_CODE_ARGS_REQ] = {"required arguments missing"},
      [ERR_CODE_OOM] = {"out of memory"},
      [ERR_CODE_NOT_FOUND] = {"command not found"},
      [ERR_CODE_ERROR] = {"error"},
      [ERR_CODE_PANIC] = {"panic"},
      [ERR_CODE_CHAN_CLOSED] = {"write to closed channel"},
      [ERR_CODE_CHAN_OK] = {"channel ok"},
  };
  return err_msg_lut[err].msg;
}
