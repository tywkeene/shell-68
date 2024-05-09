#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io/std.h>
#include <sh/api.h>
#include <util/misc.h>

SH_API_IO_IMPL char std_getch(void) { return mcInputchar(); };
SH_API_IO_IMPL void std_putch(const char c) { mcPrintchar(c); };

SH_API_IO_IMPL char *std_gets(char *buffer, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    char c = mcReadchar();

    switch (c) {
    case 0x0D:
      mcPrintchar('\r');
      mcPrintchar('\n');
      buffer[i + 1] = '\0';
      goto done;
    case 0x7F:
    case 0x08:
      i--;
      buffer[i] = '\0';
      mcPrintchar('\b');
      mcPrintchar(' ');
      mcPrintchar('\b');
      continue;
    default:
      buffer[i] = c;
      mcPrintchar(c);
    }
  }
done:
  return buffer;
}

SH_API_IO_IMPL void std_puts(const char *out) {
  for (size_t i = 0; i < strlen(out); i++)
    mcPrintchar(out[i]);
}

SH_API_IO_IMPL void std_newline(void) { printf("\n"); }

SH_API_IO_IMPL void std_set_cursor_xy(int x, int y) {
  _arg_not_used(x);
  _arg_not_used(y);
}
