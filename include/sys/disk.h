#if !defined(SYS_DISK_H)
#define SYS_DISK_H

#include <stdint.h>

void sys_init_disk(void);
void sys_list_dir(const char *path);
uint32_t sys_file_size(const char *name);
int sys_execute(const char *name, uint8_t *loadstartptr);

#endif
