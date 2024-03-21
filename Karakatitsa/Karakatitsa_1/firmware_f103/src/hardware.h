#ifndef __HARDWARE_H__
#define __HARDWARE_H__

//TIM2 -> USB_MIC / ADC

#define RLED  B,5,1,GPIO_PP50
#define YLED  B,6,1,GPIO_PP50
#define GLED  B,7,1,GPIO_PP50
//#define BTN1  A,8,0,GPIO_HIZ
//#define BTN2  A,9,0,GPIO_HIZ //используется для программирования по UART
//#define USB_PULLUP B,2,1,GPIO_PP50 //BOOT1 connected to pullup
#define USB_DP	A,12,1,GPIO_HIZ

#define UART_TX A,9, GPIO_APP50
#define UART_RX A,10,GPIO_HIZ

#define USBR	B,2,0,GPIO_PP50
#define ADC_IN  B,1,9,GPIO_ADC //?
#define RESET   C,13,0,GPIO_PP50
#define BOOT0   A,8,1,GPIO_PP50

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
