#ifndef __HARDWARE_H__
#define __HARDWARE_H__

//TIM2 -> USB_MIC / ADC

#define GLED  A,4,1,GPIO_PP50
#define RLED  A,5,1,GPIO_PP50
#define BLED  A,6,1,GPIO_PP50

#define USB_DP	A,12,1,GPIO_HIZ

#define UART_TX A,9, GPIO_APP50
#define UART_RX A,10,GPIO_HIZ

//#define USBR	B,2,0,GPIO_PP50
#define ADC_IN  A,7,7,GPIO_ADC
 
#define RESET   B,12,0,GPIO_PP50
#define BOOT0   B,13,1,GPIO_PP50

// KEY matrix
#define KBD_0	C,14,1,GPIO_PP50
#define KBD_1	B,8,1,GPIO_PP50
#define KBD_2	A,3,1,GPIO_PP50
#define KBD_3	A,2,1,GPIO_PP50

#define KBD_4	C,13,1,GPIO_PP50
#define KBD_5	B,7,1,GPIO_PP50
#define KBD_6	A,0,1,GPIO_PP50
#define KBD_7	A,1,1,GPIO_PP50

#define KBD_8	B,4,1,GPIO_PP50
#define KBD_9	B,3,1,GPIO_PP50
#define KBD_10	B,0,1,GPIO_PP50
#define KBD_11	B,1,1,GPIO_PP50

#define KBD_12	B,6,1,GPIO_PP50
#define KBD_13	B,5,1,GPIO_PP50
#define KBD_14	B,11,1,GPIO_PP50
#define KBD_15	B,10,1,GPIO_PP50

#define jtag_disable() do{\
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;\
  AFIO->MAPR = (AFIO->MAPR &~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;\
}while(0)

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
//UART1+2 - common interrupt endpoint (placeholder)
#define ENDP_TTY_CTL	3
#define ENDP_PROG_CTL	4
#define ENDP_CTL_SIZE	8 //->8
//Audio microphone (ADC on PB0)
#define ENDP_MIC	5
#define ENDP_MIC_SIZE	100 //->200
// 128 + 8 + 2*32 + 2*32 + 2*8 + 2*100

#define interface_hid	0, 1
#define interface_tty	1, 2
#define interface_progr	3, 2
#define interface_mic	5, 2
#define interface_count	7

#define ifnum(x) _marg1(x)
#define ifcnt(x) _marg2(x)

#endif
