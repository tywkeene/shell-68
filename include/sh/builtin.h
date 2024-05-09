#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdint.h>
#include <stdlib.h>

#define sizeof_array(x) (sizeof(x) / sizeof(*x))

#define check_null_args(reqc, argc, argv)                                      \
  if (argc < reqc || argv == NULL)                                             \
    return SHELL_STATUS_ARGS_REQ;

int execute_builtin(char *in);

#endif
