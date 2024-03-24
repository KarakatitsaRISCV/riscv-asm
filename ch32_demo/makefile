progname := firmware
progdir := res
files := main audiodata

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


COMMON_CH32V203 = -march=rv32imac_zicsr -DCH32V203G8
LIB_CH32V203 = $(libdir)/ch32v20x
LDFLAGS_CH32V203 = -T $(libdir)/ch32v20x/ch32v203.ld
#STARTUP_CH32V203 = startup_ch32v20x_D6
STARTUP_CH32V203 = startup_ch32v20x_D8W

COMMON_CH32V208 = -march=rv32imac_zicsr -DCH32V20x_D8W
LIB_CH32V208 = $(libdir)/ch32v20x
LDFLAGS_CH32V208 = -T $(libdir)/ch32v20x/ch32v208.ld
STARTUP_CH32V208 = startup_ch32v20x_D8W

COMMON_CH32V303 = -march=rv32imafc_zicsr -DCH32V30x_D8
LIB_CH32V303 = $(libdir)/ch32v30x
LDFLAGS_CH32V303 = -T $(libdir)/ch32v30x/ch32v303.ld
STARTUP_CH32V303 = startup_ch32v30x_D8

COMMON_CH32V307 = -march=rv32imafc_zicsr -DCH32V30x_D8C
LIB_CH32V307 = $(libdir)/ch32v30x
LDFLAGS_CH32V307 = -T $(libdir)/ch32v30x/ch32v307.ld
STARTUP_CH32V307 = startup_ch32v30x_D8C


COMMON = $(COMMON_$(MCU))
COMMON += -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -mabi=ilp32 -mcmodel=medany -static -fno-plt -fno-pic -fno-asynchronous-unwind-tables -fno-unwind-tables
COMMON += -DHSE_VALUE=$(HSE_CLOCK)
ASMFLAGS = $(COMMON)
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -Wno-main -Wstack-usage=400 -ffreestanding -Wno-unused -nostdlib -fno-builtin-printf
CFLAGS += -I$(libdir)
CFLAGS += -I$(LIB_$(MCU))/inc
CFLAGS += -I$(LIB_$(MCU))/inc/Peripheral/inc
LDFLAGS = $(COMMON)
LDFLAGS += $(LDFLAGS_$(MCU))
LDFLAGS += -Wl,--build-id=none -nostdlib

CFLAGS += -DAUDIOSRC='"_binary_src_audiodata_wav"'

files += $(STARTUP_$(MCU))

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
$(builddir)/%.o: $(libdir)/%.S
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $(ASMFLAGS) $< -o $@
	@echo
$(builddir)/audiodata.o: $(srcdir)/audiodata.wav
	mkdir -p $(builddir)
	$(CROSS_COMPILE)ld -melf32lriscv -r -b binary -o $@ $<
	$(CROSS_COMPILE)objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents $@ $@
clean:
	rm -rf $(progdir)
	rm -rf $(builddir)
size: $(frmname).elf
	$(SIZE) $(frmname).elf
	
uart_port="/dev/tty_CH32_PROG_0"

prog:	$(frmname).bin
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 1
	./wch-isp --port=$(uart_port) -p -b write $(frmname).bin
	stty -F $(uart_port) 50
	echo 'RbU' > $(uart_port)
	sleep 1
	echo 'rbuz' > $(uart_port)

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
