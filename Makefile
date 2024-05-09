ROSCO_M68K_DIR=./rosco_m68k/
SERIAL=/dev/tty.usbserial-ABSCDIGP
BAUD?=115200
MINICOM?=minicom
KERMIT=kermit

-include $(ROSCO_M68K_DIR)/code/software/software.mk

compile_commands:
	compiledb make

# upload binary to rosco (if ready and kermit present)
load: $(BINARY)
	$(KERMIT) -i -l $(SERIAL) -b $(BAUD) -s $(BINARY)

load-minicom: load
	$(MINICOM) -D $(SERIAL) -c on -R utf8 -t vt100

ROSCO_LIB_INC=./rosco_m68k/code/software/libs/build/include
ROSCO_KERN_INC=./rosco_m68k/code/firmware/rosco_m68k_firmware/kernel/include

EXTRA_LIBS=-lmachine -ldebug_stub -lvterm -lheap
EXTRA_LDFLAGS=
EXTRA_CFLAGS=-Wno-format-zero-length -Wno-format \
 			 -Wno-implicit-function-declaration \
 			 -Wunused-variable -Wno-pedantic -fbuiltin \
			 -O0 -g -I$(ROSCO_KERN_INC) -I$(ROSCO_LIB_INC) -I./include 
             
# -D_SH_DEBUG_ALL	     : Debug everything
# -D_SH_DEBUG_MALLOC     : Debug malloc
# -D_SH_DEBUG_TASK		 : Debug multitasking
# -D_SH_DEBUG_STATUS_LED : Print function addresses at function entry
# -D_SH_DEBUG_CMDLINE    : Print the cmdline input
# -D_SH_DEBUG_MUTEX		 : Debug mutex implementation
# -D_SH_DEBUG_CHAN		 : Debug channel implemention
# -D_SH_DEBUG_FN_CALL    : Print function call info
