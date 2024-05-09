#ifndef SH_STD_IO_H
#define SH_STD_IO_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sh/api.h>

SH_API_IO_IMPL char std_getch(void);
SH_API_IO_IMPL void std_putch(const char c);

SH_API_IO_IMPL char *std_gets(char *buffer, size_t n);
SH_API_IO_IMPL void std_puts(const char *out);

SH_API_IO_IMPL void std_newline(void);
SH_API_IO_IMPL void std_set_cursor_xy(int x, int y);

#endif
