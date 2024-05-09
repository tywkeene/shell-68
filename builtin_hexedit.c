#include <stdio.h>

#include <vterm.h>

#include <builtin/hexedit.h>
#include <io/term.h>
#include <sh/errcode.h>
#include <sh/print.h>
#include <sys/status_dword.h>

#define SIZE_BUF 4
#define ADDR_BUF 8

void wipe_screen_buffer(term_buffer_t *_b) {
  for (int x = 0; x < SCREEN_W; x++) {
    for (int y = 0; y < SCREEN_H; y++) {
      *_b[x][y] = ' ';
    }
  }
}

void draw_screen(term_buffer_t *_b) {
  vt_clear_screen();
  term_set_cursor_xy(0, 0);
  for (int i = 0; i < SCREEN_H; i++) {
    term_puts((char *)&_b[i]);
    term_newline();
    term_set_cursor_xy(0, i);
  }
}

void hexdump_fmt_str(char *line, uint32_t *p) {
  uint8_t one = get_dword_byte1(p);
  uint8_t two = get_dword_byte2(p);
  uint8_t three = get_dword_byte3(p);
  uint8_t four = get_dword_byte4(p);
  sprintf(line, "[ 0x%08X: | %02X %02X %02X %02X | %c %c %c %c ] ",
          (uint32_t *)p, one, two, three, four, ascii_or_dot(one),
          ascii_or_dot(two), ascii_or_dot(three), ascii_or_dot(four));
}

void update_buffer(term_buffer_t *_b, uint32_t *start, uint32_t len) {
  uint32_t *p = start;
  uint32_t *end = start + len;
  sprintf((char *)&_b[0], "   |  [0x%p] to [0x%p]  |\n", start, end);
  for (uint32_t i = 0; i < len; i++) {
    hexdump_fmt_str((char *)&_b[i], p);
    (void)*p++;
  }
}

static uint32_t test_data[80] = {
    0x1EA7BEEF, 0xDEADBEEF, 0x3EADBEEF, 0xBADB0071, 0x5EADBEEF, 0x6EADBEEF,
    0x1EA7BEEF, 0x01EA7A55, 0x9EADBEEF, 0x00A0BEE0, 0x1EA7BEEF, 0xDEADBEEF,
    0x3EADBEEF, 0xBADB0071, 0x5EADBEEF, 0x6EADBEEF, 0x1EA7BEEF, 0x01EA7A55,
    0x9EADBEEF, 0x00A0BEE0, 0x1EA7BEEF, 0xDEADBEEF, 0x3EADBEEF, 0xBADB0071,
    0x5EADBEEF, 0x6EADBEEF, 0x1EA7BEEF, 0x01EA7A55, 0x9EADBEEF, 0x00A0BEE0,
    0x1EA7BEEF, 0xDEADBEEF, 0x3EADBEEF, 0xBADB0071, 0x5EADBEEF, 0x6EADBEEF,
    0x1EA7BEEF, 0x01EA7A55, 0x9EADBEEF, 0x00A0BEE0, 0x1EA7BEEF, 0xDEADBEEF,
    0x3EADBEEF, 0xBADB0071, 0x5EADBEEF, 0x6EADBEEF, 0x1EA7BEEF, 0x01EA7A55,
    0x9EADBEEF, 0x00A0BEE0, 0x1EA7BEEF, 0xDEADBEEF, 0x3EADBEEF, 0xBADB0071,
    0x5EADBEEF, 0x6EADBEEF, 0x1EA7BEEF, 0x01EA7A55, 0x9EADBEEF, 0x00A0BEE0,
    0x1EA7BEEF, 0xDEADBEEF, 0x3EADBEEF, 0xBADB0071, 0x5EADBEEF, 0x6EADBEEF,
    0x1EA7BEEF, 0x01EA7A55, 0x9EADBEEF, 0x00A0BEE0, 0x1EA7BEEF, 0xDEADBEEF,
    0x3EADBEEF, 0xBADB0071, 0x5EADBEEF, 0x6EADBEEF, 0x1EA7BEEF, 0x01EA7A55,
    0x9EADBEEF, 0x00A0BEE0,
};

int builtin_fn_hexedit(int argc, const char **argv) {
  (void)argc;
  (void)argv;

  if (!sys_get_status(SYS_SH_TERM_MASK)) {
    sh_errorln(DEBUG_CALL_INFO,
               "VT terminal library must be initialized to run hexedit");
    return ERR_CODE_ERROR;
  }

  term_buffer_t buffer;
  wipe_screen_buffer(&buffer);
  update_buffer(&buffer, test_data, 10);

  press_the_any_key();
  draw_screen(&buffer);
  press_the_any_key();

  return ERR_CODE_OK;
}
