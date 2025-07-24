progname := firmware
progdir := res
files := main strlib

MCU := CH32V303
HSE_CLOCK := 8000000

SPEC_CFLAGS = -DMEMFUNC -DSTRMATCH_FUNC

files += tasks list port heap_4 portASM timers queue

srcdir := src
builddir := build
libdir := ../ch32_lib
mylibs := -Isrc/ -Isrc/FreeRTOS/include -Isrc/FreeRTOS/portable/GCC/RISC-V/ -Isrc/FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions/

CROSS_COMPILE=riscv64-unknown-elf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
SIZE=$(CROSS_COMPILE)size

ifneq (, $(shell which riscv64-unknown-elf-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-unknown-elf-
  GCCVER=`$(CC) --version | sed -n "1s/.* \([0-9]*[.][0-9]*[.][0-9]*\).*/\1/p"`
  GCCPATH = -L/usr/lib/gcc/riscv64-unknown-elf/$(GCCVER)/$(ARCH_$(MCU))/ilp32/
else ifneq (, $(shell which riscv64-linux-gnu-gcc 2>/dev/null))
  CROSS_COMPILE=riscv64-linux-gnu-
else
  $(error Unknown RISC-V compiler)
endif


COMMON_CH32V203 = -march=rv32imac -DCH32V203G8
LIB_CH32V203 = $(libdir)/ch32v20x
LDFLAGS_CH32V203 = -T $(libdir)/ch32v20x/ch32v203.ld
#STARTUP_CH32V203 = startup_ch32v20x_D6
STARTUP_CH32V203 = startup_ch32v20x_D8W

COMMON_CH32V208 = -march=rv32imac -DCH32V20x_D8W
LIB_CH32V208 = $(libdir)/ch32v20x
LDFLAGS_CH32V208 = -T $(libdir)/ch32v20x/ch32v208.ld
STARTUP_CH32V208 = startup_ch32v20x_D8W

COMMON_CH32V303 = -march=rv32imafc 
LIB_CH32V303 = $(libdir)/ch32v30x
#LDFLAGS_CH32V303 = -T $(libdir)/ch32v30x/ch32v303.ld
LDFLAGS_CH32V303 = -T src/ch32v303.ld
STARTUP_CH32V303 = startup_ch32v30x_D8

COMMON_CH32V307 = -march=rv32imafc_zicsr -DCH32V30x_D8C
LIB_CH32V307 = $(libdir)/ch32v30x
LDFLAGS_CH32V307 = -T $(libdir)/ch32v30x/ch32v307.ld
STARTUP_CH32V307 = startup_ch32v30x_D8C


COMMON = $(COMMON_$(MCU)) $(SPEC_CFLAGS)
COMMON += -MD -MP -MT $(builddir)/$(*F).o -MF $(builddir)/dep/$(@F).mk
COMMON += -mabi=ilp32 -mcmodel=medany
COMMON += -DHSE_VALUE=$(HSE_CLOCK)
ASMFLAGS = $(COMMON)
ASMFLAGS += $(mylibs)
CFLAGS = $(COMMON)
CFLAGS += -Wall -Os -g -Wno-main -Wstack-usage=400 -ffreestanding -Wno-unused -nostdlib -fno-builtin-printf
CFLAGS += -I$(libdir)
CFLAGS += -I$(LIB_$(MCU))/inc
CFLAGS += -I$(LIB_$(MCU))/inc/Peripheral/inc -Wstack-usage=2048
CFLAGS += $(mylibs)
LDFLAGS = $(COMMON)
LDFLAGS += $(LDFLAGS_$(MCU))
LDFLAGS += -nostdlib
LDFLAGS += -Wl,--build-id=none -nostdlib
LDFLAGS += $(GCCPATH) -lgcc

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
	
#### FreeRTOS
$(builddir)/%.o: $(srcdir)/FreeRTOS/%.c
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $< $(CFLAGS) -I$(libdir)/ch32v30x/inc/Peripheral/inc/ -include ch32v30x.h -o $@
	@echo
$(builddir)/%.o: $(srcdir)/FreeRTOS/portable/GCC/RISC-V/%.c
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $< $(CFLAGS) -I$(libdir)/ch32v30x/inc/Peripheral/inc/ -include ch32v30x.h -o $@
	@echo
$(builddir)/%.o: $(srcdir)/FreeRTOS/portable/GCC/RISC-V/%.S
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $< $(ASMFLAGS) -o $@
	@echo
$(builddir)/%.o: $(srcdir)/FreeRTOS/portable/*/%.c
	@echo -e "\nCompile " $<
	mkdir -p $(builddir)
	$(CC) -c $< $(CFLAGS) -I$(libdir)/ch32v30x/inc/Peripheral/inc/ -include ch32v30x.h -o $@
	@echo
	
clean:
	rm -rf $(progdir)
	rm -rf $(builddir)
size: $(frmname).elf
	$(SIZE) $(frmname).elf
	
wch_isp = "../wch-isp/wch-isp"
uart_port="/dev/tty_STFLASH_0"

prog:	$(frmname).bin size
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 0.1
	$(wch_isp) --port=$(uart_port) -p -b write $(frmname).bin
	stty -F $(uart_port) 50
	echo 'RbU' > $(uart_port)
	sleep 0.1
	echo 'rbuz' > $(uart_port)
info:	
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'RBU' > $(uart_port)
	echo 'rBU' > $(uart_port)
	sleep 0.1
	$(wch_isp) --port=$(uart_port) --database-path="../wch-isp/devices/" info
	#$(wch_isp) --port=$(uart_port) --database-path="../wch-isp/devices/" optionshow 'SRAM_CODE_MODE = 0'
	#$(wch_isp) --port=$(uart_port) --database-path="../wch-isp/devices/" optionbytes 'SRAM_CODE_MODE = 0'
	stty -F $(uart_port) 50
	echo 'RbU' > $(uart_port)
	sleep 0.1
	echo 'rbuz' > $(uart_port)
reset:
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'Rbu' > $(uart_port)
	echo 'rbuz' > $(uart_port)
	
pause:
	stty -F $(uart_port) 300
	stty -F $(uart_port) 50
	echo 'Rbu' > $(uart_port)
	sleep 5
	echo 'rbuz' > $(uart_port)
	
stflash:	$(frmname).bin
	stty -F /dev/tty_STFLASH_0 300
	stty -F /dev/tty_STFLASH_0 50
	$(wch_isp) --port=/dev/tty_STFLASH_0 --reset=RTS --boot0=DTR -b write $(frmname).bin
	stty -F /dev/tty_STFLASH_0 50
	echo 'rbuz' > /dev/tty_STFLASH_0
	
ft_rst:
	$(wch_isp) --port=/dev/ttyUSB0 --boot0=DTR --reset=RTS -b info
	
ftdi:	$(frmname).bin
	$(wch_isp) --port=/dev/ttyUSB0 --boot0=DTR --reset=RTS -b write $(frmname).bin
usb:	$(frmname).bin
	$(wch_isp) -b write $(frmname).bin

-include $(shell mkdir -p $(builddir)/dep) $(wildcard $(builddir)/dep/*)

.PHONY: all clean
