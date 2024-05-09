#ifndef SH_CMD_HEXEDIT
#define SH_CMD_HEXEDIT

#include <stdint.h>

#define SCREEN_W 10
#define SCREEN_H 10

typedef char term_buffer_t[SCREEN_W][SCREEN_H];

void wipe_screen_buffer(term_buffer_t *_b);
void draw_screen(term_buffer_t *_b);
void hexdump_fmt_str(char *line, uint32_t *p);
void update_buffer(term_buffer_t *_b, uint32_t *start, uint32_t len);
int cmd_fn_hexedit(int argc, const char **argv);

#endif
