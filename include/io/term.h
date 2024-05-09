#ifndef SH_TERM_H
#define SH_TERM_H

#include <sh/api.h>

#include <shell.h>

void init_vt_lib_io(void);

// These functions implement the shell_api_t io interface
SH_API_IO_IMPL char term_getch(void);
SH_API_IO_IMPL void term_putch(const char c);

SH_API_IO_IMPL char *term_gets(char *buffer, size_t n);
SH_API_IO_IMPL void term_puts(const char *out);

SH_API_IO_IMPL void term_newline(void);
SH_API_IO_IMPL void term_set_cursor_xy(int x, int y);
// end shell_api_t io interface impl

SH_API_IO_IMPL void term_prompt_input(const char *prompt, char *buf, size_t n);

#endif
