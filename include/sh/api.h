#ifndef SH_API_H
#define SH_API_H

#include <kernelapi.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include <sync/channel.h>
#include <sys/task.h>

#define SH_API_IO_IMPL

// Basically just markers to make grep'ing easier
// and to keep a better mental map of what's where
#define SH_API_PUBLIC
#define SH_API_PRIVATE static

typedef char (*io_getch_fn)(void);
typedef void (*io_putch_fn)(const char c);
typedef char *(*io_gets_fn)(char *buffer, size_t n);
typedef void (*io_puts_fn)(const char *fmt);
typedef void (*io_put_newline_fn)(void);
typedef void (*io_set_cursor_fn)(int x, int y);
typedef void (*io_prompt_fn)(const char *prompt, char *buffer, size_t n);

typedef void (*sys_disable_interrupts_fn)(void);
typedef void (*sys_enable_interrupts_fn)(void);
typedef void (*sys_halt_fn)(void);

typedef struct {
  // Data/system state //
  IKernel *k_api;

  // Multi tasking
  sh_task_list_t *tasks;
  sh_task_t *init_task;
  sh_task_t *led_task;
  sh_task_t *shell_task;

  // Methods //

  // System control methods
  sys_disable_interrupts_fn sys_disable_interrupts;
  sys_enable_interrupts_fn sys_enable_interrupts;
  sys_halt_fn sys_halt;

  // IO methods
  io_gets_fn gets;
  io_getch_fn getch;

  io_puts_fn puts;
  io_putch_fn putch;

  io_put_newline_fn put_newline;

  io_set_cursor_fn set_cursor_xy;
  io_prompt_fn prompt;
} __attribute__((aligned(4))) sh_api_t;

typedef enum {
  TERM_STD,
  TERM_VT,
} term_type_t;

SH_API_PUBLIC sh_api_t *sh_get_api(void);

SH_API_PUBLIC noreturn void sys_start();
SH_API_PUBLIC void sh_initialize_api(term_type_t term);

SH_API_PUBLIC char sh_getch(void);
SH_API_PUBLIC void sh_putch(const char c);
SH_API_PUBLIC char *sh_gets(char *buffer, size_t n);
SH_API_PUBLIC void sh_puts(const char *out);
SH_API_PUBLIC void sh_newline(void);
SH_API_PUBLIC void sh_prompt(const char *prompt, char *buffer, size_t n);

#define _do_without_interrupt(_do)                                             \
  sh_get_api()->sys_disable_interrupts();                                      \
  _do();                                                                       \
  sh_get_api()->sys_enable_interrupts();

#endif
