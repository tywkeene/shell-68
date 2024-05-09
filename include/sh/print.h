#ifndef SH_PRINT_H
#define SH_PRINT_H

#include <stdio.h>
#include <stdlib.h>

#define DEBUG_CALL_INFO __LINE__, __FILE__, __FUNCTION__
#define EMPTY_CALL_INFO (-1, "", "")

#define FN_CALL_ARGS int line, const char *file, const char *fn

#define init_message(name, _do)                                                \
  do {                                                                         \
    sh_println("[%s]: initializing...", name);                                 \
    _do();                                                                     \
    sh_println("[%s]: done", name);                                            \
  } while (0);

void sh_printf(const char *fmt, ...);
void sh_println(const char *fmt, ...);

void sh_errorln(FN_CALL_ARGS, const char *fmt, ...);
void sh_errorf(FN_CALL_ARGS, const char *fmt, ...);

void sh_debugf(FN_CALL_ARGS, const char *fmt, ...);
void sh_debugln(FN_CALL_ARGS, const char *fmt, ...);

void sh_panic(FN_CALL_ARGS, const char *fmt, ...);
#endif
