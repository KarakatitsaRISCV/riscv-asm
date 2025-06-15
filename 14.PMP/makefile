progname := firmware
progdir := res
files := main

MCU := CH32V303
HSE_CLOCK := 8000000

srcdir := src
builddir := build
libdir := ../ch32_lib

#CROSS_COMPILE=riscv64-unknown-elf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size

ifneq (, $(shell which riscv64-unknown-elf-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-unknown-elf-
else ifneq (, $(shell which riscv64-linux-gnu-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-linux-gnu-
else
  $(error Unknown RISC-V compiler)
endif

COMMON = -march=rv32imafc_zicsr -DCH32V30x_D8
COMMON += -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -mabi=ilp32 -mcmodel=medany -static -fno-plt -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables
COMMON += -DHSE_VALUE=$(HSE_CLOCK)
ASMFLAGS = $(COMMON)
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -Wno-main -Wstack-usage=400 -ffreestanding -Wno-unused -nostdlib -fno-builtin-printf
LDFLAGS = $(COMMON)
LDFLAGS += -T lib/ch32v303.ld
LDFLAGS += -Wl,--build-id=none -nostdlib

frmname = $(progdir)/$(progname)
objs = $(addprefix $(builddir)/,$(addsuffix .o,$(files)))
	
all: $(frmname).bin $(frmname).hex $(frmname).lss size


$(frmname).bin: $(frmname).elf
	$(OBJCOPY) -O binary $^ $@
$(frmname).hex: $(frmname).elf
	$(OBJCOPY) -Oihex $(frmname).elf $(frmname).hex
$(frmname).lss: $(frmname).elf
	$(OBJDUMP) -D -S $(frmname).elf > $(frmname).lss

$(frmname).elf: $(objs) $(LSCRIPT)
	mkdir -p $(progdir)
	@ echo "..linking"
	$(LD) $(LDFLAGS) $(objs) -o $@ 

$(builddir)/%.o: $(srcdir)/%.c
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $(CFLAGS) $< -o $@
	@echo
$(builddir)/%.o: $(srcdir)/%.S
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $(ASMFLAGS) $< -o $@
	@echo
clean:
	rm -rf $(progdir)
	rm -rf $(builddir)
size: $(frmname).elf
	$(SIZE) $(frmname).elf
	
uart_port="/dev/tty_CH32_PROG_0"
wch-isp="wch-isp"
#uart_port="/dev/tty_STFLASH_0"
#wch-isp="../../../2_wch/wch-isp/wch-isp"

prog:	$(frmname).bin
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 1
	$(wch-isp) --port=$(uart_port) -p -b write $(frmname).bin
	stty -F $(uart_port) 50
	echo 'RbU' > $(uart_port)
	sleep 1
	echo 'rbuz' > $(uart_port)
	
info:
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 1
	$(wch-isp) --port=$(uart_port) -p --database-path="../../" info
	stty -F $(uart_port) 50
	echo 'RbU' > $(uart_port)
	sleep 1
	echo 'rbuz' > $(uart_port)
	
reset:
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 1
	echo 'RbU' > $(uart_port)
	sleep 1
	echo 'rbuz' > $(uart_port)
	
ch32_port="/dev/tty_CH32_PROG_0"
gd32_port="/dev/tty_GD32_PROG_0"
rem_dir="/home/user/prog/test"

rem_ch:	$(frmname).bin
	scp $(frmname).bin rvstand:$(rem_dir)/test.bin
	ssh rvstand "cd $(rem_dir) ; "\
	  "stty -F $(ch32_port) 300 ;"\
	  "stty -F $(ch32_port) 50 ;"\
	  "echo 'RBU' > $(ch32_port) ;"\
	  "echo 'rBU' > $(ch32_port) ;"\
	  "sleep 1 ;"\
	  "wch-isp --port=$(ch32_port) -p -b write test.bin ;"\
	  "stty -F $(ch32_port) 50 ;"\
	  "echo 'RbU' > $(ch32_port) ;"\
	  "sleep 1 ;"\
	  "echo 'rbuz' > $(ch32_port) ;"
	  
rst_ch:
	ssh rvstand "cd $(rem_dir) ; "\
	  "stty -F $(ch32_port) 300 ;"\
	  "stty -F $(ch32_port) 50 ;"\
	  "echo 'RbU' > $(ch32_port) ;"\
	  "sleep 1 ;"\
	  "echo 'rbUz' > $(ch32_port) ;"\

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
