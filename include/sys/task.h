#ifndef SYS_TASK_H
#define SYS_TASK_H

#include <kernelapi.h>

#include <sync/atomic.h>
#include <sync/mutex.h>

#define SYS_STACK_SIZE 0x800
#define USER_STACK_SIZE 0x10000

#define MAX_RUNNING_TASKS 1024

#define TASK_LIST_SIZE (MAX_RUNNING_TASKS * sizeof(sh_task_t *))
#define TASK_MASK_NAME_SIZE 32

#define TASK_PRIORITY_0 0
#define TASK_PRIORITY_1 1
#define TASK_PRIORITY_2 2
#define TASK_PRIORITY_3 3

#define T_SIGNAL_WAKE 0x01

// indicates a function is an initialization function for a task
#define TASK_INIT_FN void

typedef void (*task_init_fn)(void);

typedef struct {
  char name[TASK_MASK_NAME_SIZE];
  Task *k_task;
  uint32_t stack_p;
  uint32_t stack_size;
  task_init_fn fn;
} sh_task_t;

typedef struct {
  volatile uint32_t index;
  sh_task_t *list;
} sh_task_list_t;

void sh_print_task_list(void);
sh_task_list_t *sh_initialize_task_list(void);
void sh_task_wait(void);
void sh_task_start(sh_task_t *task);
sh_task_t *sh_task_by_tid(uint32_t tid);
sh_task_t *sh_current_task(void);
sh_task_t *sh_create_task(const char *name, uint32_t stack_size, priority_t pri,
                          task_init_fn fn);
void sh_task_schedule(sh_task_t *task);
void sh_task_signal(sh_task_t *task, uint32_t sig);

#endif
