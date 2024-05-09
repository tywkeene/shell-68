#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include <vterm.h>

#include <io/term.h>
#include <sh/api.h>
#include <shell.h>
#include <util/misc.h>

#define ASCII_VALID_START 0x20
#define ASCII_VALID_END 0x7E

void init_vt_lib(void) { vt_init(); }

SH_API_IO_IMPL char term_getch(void) { return mcInputchar(); }
SH_API_IO_IMPL void term_putch(const char c) { mcPrintchar(c); }

SH_API_IO_IMPL void term_set_cursor_xy(int x, int y) {
  char x_str[10];
  char y_str[10];
  itoa(x, x_str, 10);
  itoa(y, y_str, 10);
  vt_move_cursor_xy(x_str, y_str);
}

SH_API_IO_IMPL void term_newline(void) {
  mcPrintchar('\r');
  mcPrintchar('\n');
  vt_move_cursor_down("1");
}

SH_API_IO_IMPL void term_puts(const char *str) {
  for (size_t i = 0; i < strlen(str); i++) {
    mcPrintchar(str[i]);
  }
}

SH_API_IO_IMPL char *term_gets(char *buff, size_t len) {
  size_t buff_index = 0;
  char in = 0;

  /* Clear the buffer before using it */
  memset(buff, 0, len);

  /* Keep getting input until we get the enter key */
  while (buff_index < len && in != 0x0d) {
    in = mcInputchar();

    /* Handle characters than can be displayed */
    if (in >= ASCII_VALID_START && in <= ASCII_VALID_END) {
      /* Add them to the input buffer */
      buff[buff_index] = in;
      buff_index++;

      /* Print them to the screen so the user can see what they are typing */
      mcPrintchar(in);
    }

    /* Handle Backspace (Win/Lin) or Del (Mac) */
    if (in == 0x7F || in == 0x08) {
      vt_backspace();

      /* Remove the deleted character from the input buffer */
      if (buff_index > 0) {
        buff_index--;
      }
      buff[buff_index] = 0;
    }

    /* Handle escape sequences */
    if (in == 0x1B && mcInputchar() == 0x5B) {
      /* Handle arrow keys */
      switch (mcInputchar()) {
      case 0x41:
        vt_move_cursor_up("1");
        break;
      case 0x42:
        vt_move_cursor_down("1");
        break;
      case 0x43:
        vt_move_cursor_left("1");
        break;
      case 0x44:
        vt_move_cursor_right("1");
        break;
      }
    }
  }
  term_newline();
  return buff;
}
