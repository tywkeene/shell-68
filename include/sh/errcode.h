#ifndef SH_ERRCODE_H
#define SH_ERRCODE_H

#include <stdint.h>

typedef uint8_t errcode_t;

enum errcode_t {
  ERR_CODE_OK = 0,          // Ok, nothing went wrong
  ERR_CODE_EXIT = 1,        // Ok, user exited
  ERR_CODE_ARGS_REQ = 2,    // Required arguments missing
  ERR_CODE_OOM = 3,         // Out of memory
  ERR_CODE_NOT_FOUND = 4,   // Command not found
  ERR_CODE_ERROR = 5,       // Error, but not fatal
  ERR_CODE_PANIC = 6,       // Something is wrong, panic (message/halt)
  ERR_CODE_CHAN_CLOSED = 7, // read from or write to closed channel
  ERR_CODE_CHAN_OK = 8,     // read from or write to channel succeeded
  NUM_ERRCODE
};

const char *errcode_str(uint8_t err);

#endif
