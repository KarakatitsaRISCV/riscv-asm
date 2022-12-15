all:
	prog:	$(frmname).bin
	stty -F /dev/tty_STFLASH_0 300
	stty -F /dev/tty_STFLASH_0 50
	echo 'RBU' > /dev/tty_STFLASH_0
	echo 'rBU' > /dev/tty_STFLASH_0
	sleep 1
	stm32flash /dev/tty_STFLASH_0 -w firmware.bin
	stty -F /dev/tty_STFLASH_0 50
	echo 'RbU' > /dev/tty_STFLASH_0
	sleep 1
	echo 'rbuz' > /dev/tty_STFLASH_0