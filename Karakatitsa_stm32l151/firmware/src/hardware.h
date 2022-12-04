#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define RLED B,5,0,GPIO_PP_VS
#define GLED B,6,0,GPIO_PP_VS
#define BLED B,4,0,GPIO_PP_VS

#define ADC_IN  B,0,8,GPIO_ANA
#define RESET   A,8,0,GPIO_PP_VS
#define BOOT0   B,3,1,GPIO_PP_VS

#include "pinmacro.h"
  
//------ USB -------------------
//PMA_size = 128
//EP0_size = 8

//Custom HID
//define ENDP_HID (none)
// /dev/ttyACM0 - TTY
#define ENDP_TTY_IN	1
#define ENDP_TTY_OUT	1
#define ENDP_TTY_SIZE	32 //->64
// /dev/ttyACM1 - PROGR
#define ENDP_PROG_IN	2
#define ENDP_PROG_OUT	2
#define ENDP_PROG_SIZE	32 //->64
// /dev/ttyACM2 - LOG
#define ENDP_LOG_IN	3
#define ENDP_LOG_OUT	3
#define ENDP_LOG_SIZE	32
//UART1+2 - common interrupt endpoint (placeholder)
#define ENDP_TTY_CTL	4
#define ENDP_PROG_CTL	5
#define ENDP_LOG_CTL	6
#define ENDP_CTL_SIZE	8 //->8
//Audio microphone (ADC on PB0)
#define ENDP_MIC	7
#define ENDP_MIC_SIZE	100 //->200
// 128 + 8 + 2*32 + 2*32 + 2*32 + 3*8 + 2*100 = 456

#define interface_hid	0, 1
#define interface_tty	1, 2
#define interface_progr	3, 2
#define interface_log	5, 3
#define interface_mic	7, 2
#define interface_count	9

#define ifnum(x) _marg1(x)
#define ifcnt(x) _marg2(x)

#endif
