progname := firmware
progdir := res
files := main
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
ARCH := rv32imac_zicsr_zifencei

COMMON = -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -march=$(ARCH) -mabi=ilp32 -mcmodel=medany -static -gdwarf-2 -fno-plt -fno-pic -static
COMMON += -DHXTAL_VALUE=$(HSE_CLOCK)
ASMFLAGS = $(COMMON)
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -Wno-main -Wstack-usage=1024 -ffreestanding -Wno-unused -nostdlib -fno-builtin-printf
CFLAGS += -I$(libdir)/Firmware/GD32VF103_standard_peripheral/Include/
CFLAGS += -I$(libdir)/Firmware/GD32VF103_standard_peripheral/
CFLAGS += -I$(libdir)/Firmware/RISCV/drivers/
CFLAGS += -I$(libdir)/Firmware/
LDFLAGS := $(COMMON) -Wl,--build-id=none -nostdlib -T lib/gd32vf103cbt6.ld

frmname = $(progdir)/$(progname)
files += startup n200_func
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

ch32_port="/dev/tty_CH32_PROG_0"
gd32_port="/dev/tty_GD32_PROG_0"
	
prog:	$(frmname).bin
	stty -F $(gd32_port) 300
	stty -F $(gd32_port) 50
	echo 'RBU' > $(gd32_port)
	echo 'rBU' > $(gd32_port)
	sleep 1
	stm32flash $(gd32_port) -w $(frmname).bin
	stty -F $(gd32_port) 50
	echo 'RbU' > $(gd32_port)
	sleep 1
	echo 'rbuz' > $(gd32_port)
reset:
	stty -F $(gd32_port) 300
	stty -F $(gd32_port) 50
	echo 'RBU' > $(gd32_port)
	echo 'rBU' > $(gd32_port)
	sleep 1
	stty -F $(gd32_port) 50
	echo 'RbU' > $(gd32_port)
	sleep 1
	echo 'rbuz' > $(gd32_port)
prog_st: $(frmname).bin
	stm32flash /dev/ttyUSB0 -w $(frmname).bin
dfu: $(frmname).bin
	dfu-util -a 0 -d 28e9:0189 -s 0x08000000 -D $(frmname).bin
	

rem_dir="/home/user/prog/test"

rem:	$(frmname).bin
	scp $(frmname).bin rvstand:$(rem_dir)/test.bin
	ssh rvstand "cd $(rem_dir) ; "\
	  "stty -F $(gd32_port) 300 ;"\
	  "stty -F $(gd32_port) 50 ;"\
	  "echo 'RBU' > $(gd32_port) ;"\
	  "echo 'rBU' > $(gd32_port) ;"\
	  "sleep 1 ;"\
	  "stm32flash $(gd32_port) -w test.bin ;"\
	  "stty -F $(gd32_port) 50 ;"\
	  "echo 'RbU' > $(gd32_port) ;"\
	  "sleep 1 ;"\
	  "echo 'rbuz' > $(gd32_port) ;"
	  
rem_rst:
	ssh rvstand "cd $(rem_dir) ; "\
	  "stty -F $(gd32_port) 300 ;"\
	  "stty -F $(gd32_port) 50 ;"\
	  "echo 'RbU' > $(gd32_port) ;"\
	  "sleep 1 ;"\
	  "echo 'rbUz' > $(gd32_port) ;"\

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
