#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sh/api.h>
#include <sh/print.h>
#include <shell.h>
#include <sys/task.h>

sh_task_t *idle_task;

void sh_print_task_list(void) {
  sh_api_t *api = sh_get_api();

  for (uint32_t i = 0; i < api->tasks->index; i++) {
    sh_task_t t = api->tasks->list[i];
    sh_println("task_id: %u [%s] k_task: 0x%p stack_p: 0x%p stack_size: %u "
               "init_fn: 0x%p",
               t.k_task->tid, t.name, t.k_task, t.stack_p, t.stack_size, t.fn);
  }
}

TASK_INIT_FN idle_fn(void) {
  while (true) {
    __asm__ volatile("stop   #0x2000\n\t");
  }
}

sh_task_list_t *sh_initialize_task_list(void) {
  sh_println("initializing task list...");
  sh_api_t *api = sh_get_api();
  api->sys_disable_interrupts();

  sh_task_list_t *sh_tasks =
      api->k_api->alloc_sys_object(sizeof(sh_task_list_t *));
  sh_tasks->index = 0;
  sh_tasks->list = (sh_task_t *)api->k_api->mem_alloc(TASK_LIST_SIZE);
  memset(sh_tasks->list, 0, TASK_LIST_SIZE);

  idle_task = sh_create_task("idle", SYS_STACK_SIZE, TASK_PRIORITY_1, idle_fn);
  sh_task_schedule(idle_task);

  api->sys_enable_interrupts();
  return sh_tasks;
}

void sh_task_wait(void) { sh_get_api()->k_api->task_wait(T_SIGNAL_WAKE); }

void sh_task_start(sh_task_t *task) {
  sh_println("starting task [%s]", task->name);
  sh_get_api()->k_api->start(task->stack_p, task->stack_size, task->fn,
                             idle_task->stack_p, idle_task->stack_size,
                             idle_task->fn);
}

sh_task_t *sh_create_task(const char *name, uint32_t stack_size, priority_t pri,
                          task_init_fn fn) {
#if defined(_SH_DEBUG_TASK)
  sh_debugln(DEBUG_CALL_INFO,
             "(name='%s', stack_size=%u, pri=%u, task_init_fn=0x%p)", name,
             stack_size, pri, fn);
#endif
  sh_api_t *api = sh_get_api();

  sh_println("creating task [%s]", name);
  sh_task_t *task = (sh_task_t *)api->k_api->mem_alloc(sizeof(sh_task_t *));
  strncpy(task->name, name, TASK_MASK_NAME_SIZE);
  task->stack_size = stack_size;
  task->stack_p = api->k_api->mem_alloc(stack_size);
  task->k_task = (Task *)api->k_api->alloc_sys_object(TASK_SLAB_BLOCKS);
  task->fn = fn;

  uint32_t tid = api->tasks->index;
  api->k_api->task_init(task->k_task, tid, pri);
  api->tasks->list[tid] = *task;
  api->tasks->index++;

  return task;
}

sh_task_t *sh_task_by_tid(uint32_t tid) {
  return &sh_get_api()->tasks->list[tid];
}

sh_task_t *sh_current_task(void) {
  Task *curr = sh_get_api()->k_api->task_current();
  return sh_task_by_tid(curr->tid);
}

void sh_task_schedule(sh_task_t *task) {
#if defined(_SH_DEBUG_TASK)
  sh_debugln(DEBUG_CALL_INFO, "(task=0x%p) [%s]", task, task->name);
#endif
  sh_get_api()->k_api->task_schedule(task->k_task, task->stack_p,
                                     task->stack_size, task->fn);
}

void sh_task_signal(sh_task_t *task, uint32_t sig) {
#if defined(_SH_DEBUG_TASK)
  uint32_t tid = sh_get_api()->k_api->task_current()->tid;
  sh_task_t *from = sh_task_by_tid(tid);
  sh_debugln(DEBUG_CALL_INFO, "(task=0x%p, sig=%u) [from='%s'] -> [to='%s']",
             task, sig, from->name, task->name);
#endif
  sh_get_api()->k_api->task_signal(task->k_task, sig);
}
