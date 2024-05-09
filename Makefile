SERIAL=/dev/tty.usbserial-ABSCDIGP
BAUD?=115200
MINICOM?=minicom
KERMIT=kermit
ROSCO_M68K_DIR=./rosco_m68k
ROSCO_LIB_INC=$(ROSCO_M68K_DIR)/code/software/libs/build/include
ROSCO_KERN_INC=$(ROSCO_M68K_DIR)/code/firmware/rosco_m68k_firmware/kernel/include
EXTRA_LIBS=-lmachine -ldebug_stub -lvterm -lheap
EXTRA_LDFLAGS=
EXTRA_CFLAGS=-Wno-format-zero-length -Wno-format \
 			 -Wno-implicit-function-declaration \
 			 -Wunused-variable -Wno-pedantic -fbuiltin \
			 -O0 -g -I$(ROSCO_KERN_INC) -I$(ROSCO_LIB_INC) \
             -I./include

-include $(ROSCO_M68K_DIR)/code/software/software.mk
load-minicom: load
	$(MINICOM) -D $(SERIAL) -c on -R utf8 -t vt100

# -D_SH_DEBUG_ALL	     : Debug everything
# -D_SH_DEBUG_MALLOC     : Debug malloc
# -D_SH_DEBUG_TASK		 : Debug multitasking
# -D_SH_DEBUG_STATUS_LED : Print function addresses at function entry
# -D_SH_DEBUG_CMDLINE    : Print the cmdline input
# -D_SH_DEBUG_MUTEX		 : Debug mutex implementation
# -D_SH_DEBUG_CHAN		 : Debug channel implemention
# -D_SH_DEBUG_FN_CALL    : Print function call info
