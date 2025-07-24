progname := firmware
progdir := res
files := main startup n200_func os

HSE_CLOCK := 8000000

srcdir := src
builddir := build
libdir := lib

#CROSS_COMPILE=riscv64-unknown-elf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size

COMMON = -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -march=rv32imac_zicsr -mabi=ilp32 -mcmodel=medany -static -fno-plt -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables
COMMON += -DHXTAL_VALUE=$(HSE_CLOCK)
ASMFLAGS = $(COMMON)
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -Wno-main -Wstack-usage=400 -ffreestanding -Wno-unused -nostdlib -fno-builtin-printf
CFLAGS += -I$(libdir)/Firmware/GD32VF103_standard_peripheral/Include/
CFLAGS += -I$(libdir)/Firmware/GD32VF103_standard_peripheral/
CFLAGS += -I$(libdir)/Firmware/RISCV/drivers/
CFLAGS += -I$(libdir)/Firmware/
LDFLAGS := $(COMMON) -Wl,--build-id=none -nostdlib -T lib/gd32vf103cbt6.ld

ifneq (, $(shell which riscv64-unknown-elf-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-unknown-elf-
else ifneq (, $(shell which riscv64-linux-gnu-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-linux-gnu-
else
  $(error Unknown RISC-V compiler)
endif

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
	mkdir -p $(builddir)
	$(CC) -c $(CFLAGS) $< -o $@
$(builddir)/%.o: $(srcdir)/%.S
	mkdir -p $(builddir)
	$(CC) -c $(ASMFLAGS) $< -o $@
$(builddir)/%.o: $(libdir)/%.S
	mkdir -p $(builddir)
	$(CC) -c $(ASMFLAGS) $< -o $@
$(builddir)/n200_func.o: $(libdir)/Firmware/RISCV/drivers/n200_func.c
	mkdir -p $(builddir)
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf $(progdir)
	rm -rf $(builddir)
size: $(frmname).elf
	$(SIZE) $(frmname).elf

uart_port="/dev/tty_GD32_PROG_0"
	
prog:	$(frmname).bin
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 1
	stm32flash $(uart_port) -w $(frmname).bin
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
	stty -F $(uart_port) 50
	echo 'RbU' > $(uart_port)
	sleep 1
	echo 'rbuz' > $(uart_port)
prog_st: $(frmname).bin
	stm32flash /dev/ttyUSB0 -w $(frmname).bin
dfu: $(frmname).bin
	dfu-util -a 0 -d 28e9:0189 -s 0x08000000 -D $(frmname).bin

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
