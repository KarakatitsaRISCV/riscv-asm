progname := firmware
progdir := res
files := main startup n200_func os

HSE_CLOCK := 8000000

srcdir := src
builddir := build
libdir := lib

CROSS_COMPILE=riscv64-unknown-elf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size

COMMON = -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -march=rv32imac_zicsr -mabi=ilp32 -mcmodel=medany
COMMON += -DHXTAL_VALUE=$(HSE_CLOCK)
ASMFLAGS = $(COMMON)
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -Wno-main -Wstack-usage=400 -ffreestanding -Wno-unused -nostdlib -fno-builtin-printf
CFLAGS += -I$(libdir)/Firmware/GD32VF103_standard_peripheral/Include/
CFLAGS += -I$(libdir)/Firmware/GD32VF103_standard_peripheral/
CFLAGS += -I$(libdir)/Firmware/RISCV/drivers/
CFLAGS += -I$(libdir)/Firmware/
LDFLAGS := -march=rv32imac_zicsr -mabi=ilp32 -mcmodel=medany -nostdlib -T $(libdir)/gd32vf103cbt6.ld

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
prog:	$(frmname).bin
	stty -F /dev/tty_STFLASH_0 300
	stty -F /dev/tty_STFLASH_0 50
	echo 'RBU' > /dev/tty_STFLASH_0
	echo 'rBU' > /dev/tty_STFLASH_0
	sleep 1
	stm32flash /dev/tty_STFLASH_0 -w $(frmname).bin
	stty -F /dev/tty_STFLASH_0 50
	echo 'RBU' > /dev/tty_STFLASH_0
	sleep 1
	echo 'rbuz' > /dev/tty_STFLASH_0
reset:
	stty -F /dev/tty_STFLASH_0 300
	stty -F /dev/tty_STFLASH_0 50
	echo 'RBU' > /dev/tty_STFLASH_0
	echo 'rBU' > /dev/tty_STFLASH_0
	sleep 1
	stty -F /dev/tty_STFLASH_0 50
	echo 'RBU' > /dev/tty_STFLASH_0
	sleep 1
	echo 'rbuz' > /dev/tty_STFLASH_0
prog_st: $(frmname).bin
	stm32flash /dev/ttyUSB0 -w $(frmname).bin
dfu: $(frmname).bin
	dfu-util -a 0 -d 28e9:0189 -s 0x08000000 -D $(frmname).bin

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
