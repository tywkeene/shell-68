#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sh/errcode.h>
#include <sh/print.h>
#include <sys/disk.h>
#include <sys/status_dword.h>
#include <util/misc.h>

#include <machine.h>
#include <sdfat.h>

#define MAX_BIN_NAMELEN FATFS_MAX_LONG_FILENAME // full name length
static char current_dir[MAX_BIN_NAMELEN]; // current dir string (root = "")

void sys_init_disk(void) {
  if (SD_check_support() == false) {
    sh_errorln(DEBUG_CALL_INFO, "SD support not found. skipping intialization");
    sys_unset_status(SYS_SH_DISK_MASK);
    return;
  }

  if (SD_FAT_initialize() == false) {
    sh_errorln(DEBUG_CALL_INFO, "failed to initialize disk");
    sys_unset_status(SYS_SH_DISK_MASK);
    return;
  }
  sh_println("disk ok");
  current_dir[0] = '/';
  current_dir[1] = '\0';
  sys_set_status(SYS_SH_DISK_MASK);
}

static char *fullpath(const char *path) {
  static char filename[FATFS_MAX_LONG_FILENAME];
  memset(filename, 0, sizeof(filename));
  if (path[0] == '/')
    strncpy(filename, path, sizeof(filename) - 1);
  else
    snprintf(filename, sizeof(filename) - 1, "/%s%s%s", current_dir,
             strlen(current_dir) > 0 ? "/" : "", path);
  return filename;
}

void sys_list_dir(const char *path) {
  FL_DIR dirstat;
  if (fl_opendir(fullpath(path), &dirstat)) {
    struct fs_dir_ent dirent;
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if (dirent.filename[0] == '.')
        continue;
      if (!dirent.is_dir) {
        printf("\t%s (%u bytes)\n", dirent.filename, dirent.size);
      } else {
        printf("\t%s\n", dirent.filename);
      }
    }
  }
  fl_closedir(&dirstat);
}

uint32_t sys_file_size(const char *name) {
  const char *filename = fullpath(name);
  FL_DIR dirstat;
  if (fl_opendir(filename, &dirstat)) {
    struct fs_dir_ent dirent;
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if (dirent.filename[0] == '.')
        continue;
      if (!dirent.is_dir && strncmp(name, dirent.filename, strlen(name)) == 0) {
        return dirent.size;
      }
    }
  }
  return 0;
}

void insert_jmp_back(uint8_t *ptr, uint32_t size, uint32_t *addr) {
  // 30 7C move.l  #$00000004,a0
  // 4E D0 jmp     (a0)
  uint8_t one = get_dword_byte1(addr);
  uint8_t two = get_dword_byte2(addr);
  uint8_t three = get_dword_byte3(addr);
  uint8_t four = get_dword_byte4(addr);

  uint8_t jmp_code[8] = {0x30, 0x7C, one, two, three, four, 0x4E, 0xD0};
  uint8_t *p = (ptr + size + 1);
  for (uint32_t i = 0; i < sizeof_array(jmp_code); i++)
    *p++ = jmp_code[i];
  sh_hex_dump((uint32_t *)&p + size, (uint32_t *)&p + 20);
}

int sys_execute(const char *name, uint8_t *loadstartptr) {
  const char *filename = fullpath(name);
  sh_printf("Loading '%s'...", filename);

  if (loadstartptr != (uint8_t *)_LOAD_ADDRESS) {
    printf(" to address 0x%08x\n", (int)loadstartptr);
  }

  void *file = fl_fopen(filename, "r");

  if (!file) {
    char *ext = strrchr(filename, '.');
    if (ext) {
      for (int i = 1; i < 4; i++) {
        if (ext[i] == '\0') {
          ext[i] = ' ';
        }
      }
      ext[4] = '\0';
    }
    file = fl_fopen(filename, "r");
  }

  if (file != NULL) {
    int c = 0;
    int b = 0;
    uint8_t *loadptr = loadstartptr;
    uint8_t *endptr = (uint8_t *)_INITIAL_STACK;
    while (loadptr < endptr && (c = fl_fread(loadptr, 1, 512, file)) > 0) {
      loadptr += c;
      /* period every 4KiB, does not noticeably affect speed */
      if (++b == 8) {
        mcPrintchar('.');
        b = 0;
      }
    }

    fl_fclose(file);
    unsigned int bytes = loadptr - loadstartptr;

    extern void sh_main_shell_loop(void);
    insert_jmp_back(loadstartptr, bytes, (uint32_t *)&sh_main_shell_loop);

    if (c == EOF) {
      sh_println("loaded %u bytes to 0x%p", bytes, (uint32_t *)loadstartptr);
      sh_println("ready...");
      press_the_any_key();
      __asm__ __volatile__("move.l %0,%%a0\n" : : "g"(loadstartptr));
      __asm__ __volatile__(" jmp (%%a0)\n" : : :);
      __builtin_unreachable();
    }
  } else {
    sh_errorln(DEBUG_CALL_INFO, "open file '%s' failed", filename);
    return ERR_CODE_FD_NOT_FOUND;
  }
  return ERR_CODE_OK;
}
