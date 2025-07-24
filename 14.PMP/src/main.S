.equ MIE,	(1<<3)
.equ MPIE,	(1<<7)
.equ MPP_U,	(0b00<<11)
.equ MPP_M,	(0b11<<11)

.equ PFIC_BASE,		0xE000E000
.equ PFIC_IENR1,	0x100

.equ GPIOA,		0x40010800
.equ GPIOB,		0x40010C00
.equ GPIO_CFGLR,0x00
.equ GPIO_CFGHR,0x04
.equ GPIO_INDR,	0x08
.equ GPIO_OUTDR,0x0C
.equ GPIO_BSHR,	0x10

.equ GPIO_OUT,  0b0011
.equ GPIO_INP,  0b0100
.equ GPIO_ALT,	0b1011

.equ USART1,		0x40013800
.equ USART_STATR,	0
.equ USART_DATAR,	4
.equ USART_BRR,		8
.equ USART_CTLR1,	0x0C

.equ USART_STATR_TXE,	(1<<7)
.equ USART_STATR_RXNE,	(1<<5)
.equ USART_CTLR1_UE,	(1<<13)
.equ USART_CTLR1_TE,	(1<<3)
.equ USART_CTLR1_RE,	(1<<2)
.equ USART_CTLR1_TXEIE,	(1<<7)

.equ RCC_APB2ENR_AFEN,		(1<<0)
.equ RCC_APB2ENR_PAEN,		(1<<2)
.equ RCC_APB2ENR_PBEN,		(1<<3)
.equ RCC_APB2ENR_USART1EN,	(1<<14)
.equ RCC_APB2ENR,			0x40021018

#PORTA
.equ USART_TX,	9
.equ USART_RX,	10

.equ GLED, 11 #PA11
.equ RLED, 4  #~PB4
#.equ GLED, 7  #PB7
#.equ RLED, 5  #PB5
#.equ YLED, 6  #PB6

.equ BTN, 0

.macro push regs:vararg
  .equ _pushpop_cnt, 0
  .irp idx, \regs
    .equ _pushpop_cnt, (_pushpop_cnt-4)
  .endr
  addi sp, sp, _pushpop_cnt
  .irp idx, \regs
    .equ _pushpop_cnt, (_pushpop_cnt+4)
    sw \idx, -_pushpop_cnt(sp)
  .endr
.endm

.macro pop regs:vararg
  .equ _pushpop_cnt, 0
  .irp idx, \regs
    .equ _pushpop_cnt, (_pushpop_cnt-4)
  .endr
  .equ _pushpop_cnt2, _pushpop_cnt
  .irp idx, \regs
    .equ _pushpop_cnt, (_pushpop_cnt+4)
    lw \idx, -_pushpop_cnt(sp)
  .endr
  addi sp, sp, -_pushpop_cnt2
.endm

.macro uart_puts_P str:vararg
.section .rodata
  1:
  .irp idx, \str
    .ascii "\idx"
  .endr
  .byte 0
.text
  la a0, 1b
  call uart_puts
.endm



	.section	.init,"ax",@progbits
	.global	_start
	.align	1
_start:
	j	handle_reset
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00000013
	.word 0x00100073
    .section    .vector,"ax",@progbits
    .align  1
_vector_base:
    .option norvc;
    .word   _start
    .word   0
    .word   NMI_Handler                /* NMI */
.word trap_hardfault /* Hard Fault */
    .word   0
.word trap_ecall_M /* Ecall M Mode */
    .word   0
    .word   0
.word trap_ecall_U /* Ecall U Mode */
    .word   Break_Point_Handler        /* Break Point */
    .word   0
    .word   0
    .word   SysTick_Handler            /* SysTick */
    .word   0
    .word   SW_Handler                 /* SW */
    .word   0
    /* External Interrupts */
    .word   WWDG_IRQHandler            /* Window Watchdog */
    .word   PVD_IRQHandler             /* PVD through EXTI Line detect */
    .word   TAMPER_IRQHandler          /* TAMPER */
    .word   RTC_IRQHandler             /* RTC */
    .word   FLASH_IRQHandler           /* Flash */
    .word   RCC_IRQHandler             /* RCC */
    .word   EXTI0_IRQHandler           /* EXTI Line 0 */
    .word   EXTI1_IRQHandler           /* EXTI Line 1 */
    .word   EXTI2_IRQHandler           /* EXTI Line 2 */
    .word   EXTI3_IRQHandler           /* EXTI Line 3 */
    .word   EXTI4_IRQHandler           /* EXTI Line 4 */
    .word   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
    .word   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
    .word   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
    .word   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
    .word   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
    .word   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
    .word   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
    .word   ADC1_2_IRQHandler          /* ADC1_2 */
    .word   USB_HP_CAN1_TX_IRQHandler  /* USB HP and CAN1 TX */
    .word   USB_LP_CAN1_RX0_IRQHandler /* USB LP and CAN1RX0 */
    .word   CAN1_RX1_IRQHandler        /* CAN1 RX1 */
    .word   CAN1_SCE_IRQHandler        /* CAN1 SCE */
    .word   EXTI9_5_IRQHandler         /* EXTI Line 9..5 */
    .word   TIM1_BRK_IRQHandler        /* TIM1 Break */
    .word   TIM1_UP_IRQHandler         /* TIM1 Update */
    .word   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
    .word   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
    .word   TIM2_IRQHandler            /* TIM2 */
    .word   TIM3_IRQHandler            /* TIM3 */
    .word   TIM4_IRQHandler            /* TIM4 */
    .word   I2C1_EV_IRQHandler         /* I2C1 Event */
    .word   I2C1_ER_IRQHandler         /* I2C1 Error */
    .word   I2C2_EV_IRQHandler         /* I2C2 Event */
    .word   I2C2_ER_IRQHandler         /* I2C2 Error */
    .word   SPI1_IRQHandler            /* SPI1 */
    .word   SPI2_IRQHandler            /* SPI2 */
    .word   USART1_IRQHandler          /* USART1 */
    .word   USART2_IRQHandler          /* USART2 */
    .word   USART3_IRQHandler          /* USART3 */
    .word   EXTI15_10_IRQHandler       /* EXTI Line 15..10 */
    .word   RTCAlarm_IRQHandler        /* RTC Alarm through EXTI Line */
    .word   0
    .word   TIM8_BRK_IRQHandler        /* TIM8 Break */
    .word   TIM8_UP_IRQHandler         /* TIM8 Update */
    .word   TIM8_TRG_COM_IRQHandler    /* TIM8 Trigger and Commutation */
    .word   TIM8_CC_IRQHandler         /* TIM8 Capture Compare */
    .word   RNG_IRQHandler             /* RNG */
    .word   FSMC_IRQHandler            /* FSMC */
    .word   SDIO_IRQHandler            /* SDIO */
    .word   TIM5_IRQHandler            /* TIM5 */
    .word   SPI3_IRQHandler            /* SPI3 */
    .word   UART4_IRQHandler           /* UART4 */
    .word   UART5_IRQHandler           /* UART5 */
    .word   TIM6_IRQHandler            /* TIM6 */
    .word   TIM7_IRQHandler            /* TIM7 */
    .word   DMA2_Channel1_IRQHandler   /* DMA2 Channel 1 */
    .word   DMA2_Channel2_IRQHandler   /* DMA2 Channel 2 */
    .word   DMA2_Channel3_IRQHandler   /* DMA2 Channel 3 */
    .word   DMA2_Channel4_IRQHandler   /* DMA2 Channel 4 */
    .word   DMA2_Channel5_IRQHandler   /* DMA2 Channel 5 */
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   OTG_FS_IRQHandler          /* OTGFS */
    .word   0
    .word   0
    .word   0
    .word   UART6_IRQHandler           /* UART6 */
    .word   UART7_IRQHandler           /* UART7 */
    .word   UART8_IRQHandler           /* UART8 */
    .word   TIM9_BRK_IRQHandler        /* TIM9 Break */
    .word   TIM9_UP_IRQHandler         /* TIM9 Update */
    .word   TIM9_TRG_COM_IRQHandler    /* TIM9 Trigger and Commutation */
    .word   TIM9_CC_IRQHandler         /* TIM9 Capture Compare */
    .word   TIM10_BRK_IRQHandler       /* TIM10 Break */
    .word   TIM10_UP_IRQHandler        /* TIM10 Update */
    .word   TIM10_TRG_COM_IRQHandler   /* TIM10 Trigger and Commutation */
    .word   TIM10_CC_IRQHandler        /* TIM10 Capture Compare */
    .word   DMA2_Channel6_IRQHandler   /* DMA2 Channel 6 */
    .word   DMA2_Channel7_IRQHandler   /* DMA2 Channel 7 */
    .word   DMA2_Channel8_IRQHandler   /* DMA2 Channel 8 */
    .word   DMA2_Channel9_IRQHandler   /* DMA2 Channel 9 */
    .word   DMA2_Channel10_IRQHandler  /* DMA2 Channel 10 */
    .word   DMA2_Channel11_IRQHandler  /* DMA2 Channel 11 */

    .option rvc;

    .section    .text.vector_handler, "ax", @progbits
    .weak   NMI_Handler                /* NMI */
    .weak   HardFault_Handler          /* Hard Fault */
    .weak   Ecall_M_Mode_Handler       /* Ecall M Mode */
    .weak   Ecall_U_Mode_Handler       /* Ecall U Mode */
    .weak   Break_Point_Handler        /* Break Point */
    .weak   SysTick_Handler            /* SysTick */
    .weak   SW_Handler                 /* SW */
    .weak   WWDG_IRQHandler            /* Window Watchdog */
    .weak   PVD_IRQHandler             /* PVD through EXTI Line detect */
    .weak   TAMPER_IRQHandler          /* TAMPER */
    .weak   RTC_IRQHandler             /* RTC */
    .weak   FLASH_IRQHandler           /* Flash */
    .weak   RCC_IRQHandler             /* RCC */
    .weak   EXTI0_IRQHandler           /* EXTI Line 0 */
    .weak   EXTI1_IRQHandler           /* EXTI Line 1 */
    .weak   EXTI2_IRQHandler           /* EXTI Line 2 */
    .weak   EXTI3_IRQHandler           /* EXTI Line 3 */
    .weak   EXTI4_IRQHandler           /* EXTI Line 4 */
    .weak   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
    .weak   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
    .weak   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
    .weak   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
    .weak   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
    .weak   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
    .weak   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
    .weak   ADC1_2_IRQHandler          /* ADC1_2 */
    .weak   USB_HP_CAN1_TX_IRQHandler  /* USB HP and CAN1 TX */
    .weak   USB_LP_CAN1_RX0_IRQHandler /* USB LP and CAN1RX0 */
    .weak   CAN1_RX1_IRQHandler        /* CAN1 RX1 */
    .weak   CAN1_SCE_IRQHandler        /* CAN1 SCE */
    .weak   EXTI9_5_IRQHandler         /* EXTI Line 9..5 */
    .weak   TIM1_BRK_IRQHandler        /* TIM1 Break */
    .weak   TIM1_UP_IRQHandler         /* TIM1 Update */
    .weak   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
    .weak   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
    .weak   TIM2_IRQHandler            /* TIM2 */
    .weak   TIM3_IRQHandler            /* TIM3 */
    .weak   TIM4_IRQHandler            /* TIM4 */
    .weak   I2C1_EV_IRQHandler         /* I2C1 Event */
    .weak   I2C1_ER_IRQHandler         /* I2C1 Error */
    .weak   I2C2_EV_IRQHandler         /* I2C2 Event */
    .weak   I2C2_ER_IRQHandler         /* I2C2 Error */
    .weak   SPI1_IRQHandler            /* SPI1 */
    .weak   SPI2_IRQHandler            /* SPI2 */
    .weak   USART1_IRQHandler          /* USART1 */
    .weak   USART2_IRQHandler          /* USART2 */
    .weak   USART3_IRQHandler          /* USART3 */
    .weak   EXTI15_10_IRQHandler       /* EXTI Line 15..10 */
    .weak   RTCAlarm_IRQHandler        /* RTC Alarm through EXTI Line */
    .weak   TIM8_BRK_IRQHandler        /* TIM8 Break */
    .weak   TIM8_UP_IRQHandler         /* TIM8 Update */
    .weak   TIM8_TRG_COM_IRQHandler    /* TIM8 Trigger and Commutation */
    .weak   TIM8_CC_IRQHandler         /* TIM8 Capture Compare */
    .weak   RNG_IRQHandler             /* RNG */
    .weak   FSMC_IRQHandler            /* FSMC */
    .weak   SDIO_IRQHandler            /* SDIO */
    .weak   TIM5_IRQHandler            /* TIM5 */
    .weak   SPI3_IRQHandler            /* SPI3 */
    .weak   UART4_IRQHandler           /* UART4 */
    .weak   UART5_IRQHandler           /* UART5 */
    .weak   TIM6_IRQHandler            /* TIM6 */
    .weak   TIM7_IRQHandler            /* TIM7 */
    .weak   DMA2_Channel1_IRQHandler   /* DMA2 Channel 1 */
    .weak   DMA2_Channel2_IRQHandler   /* DMA2 Channel 2 */
    .weak   DMA2_Channel3_IRQHandler   /* DMA2 Channel 3 */
    .weak   DMA2_Channel4_IRQHandler   /* DMA2 Channel 4 */
    .weak   DMA2_Channel5_IRQHandler   /* DMA2 Channel 5 */
    .weak   OTG_FS_IRQHandler          /* OTGFS */
    .weak   UART6_IRQHandler           /* UART6 */
    .weak   UART7_IRQHandler           /* UART7 */
    .weak   UART8_IRQHandler           /* UART8 */
    .weak   TIM9_BRK_IRQHandler        /* TIM9 Break */
    .weak   TIM9_UP_IRQHandler         /* TIM9 Update */
    .weak   TIM9_TRG_COM_IRQHandler    /* TIM9 Trigger and Commutation */
    .weak   TIM9_CC_IRQHandler         /* TIM9 Capture Compare */
    .weak   TIM10_BRK_IRQHandler       /* TIM10 Break */
    .weak   TIM10_UP_IRQHandler        /* TIM10 Update */
    .weak   TIM10_TRG_COM_IRQHandler   /* TIM10 Trigger and Commutation */
    .weak   TIM10_CC_IRQHandler        /* TIM10 Capture Compare */
    .weak   DMA2_Channel6_IRQHandler   /* DMA2 Channel 6 */
    .weak   DMA2_Channel7_IRQHandler   /* DMA2 Channel 7 */
    .weak   DMA2_Channel8_IRQHandler   /* DMA2 Channel 8 */
    .weak   DMA2_Channel9_IRQHandler   /* DMA2 Channel 9 */
    .weak   DMA2_Channel10_IRQHandler  /* DMA2 Channel 10 */
    .weak   DMA2_Channel11_IRQHandler  /* DMA2 Channel 11 */

NMI_Handler:  1:  j 1b
HardFault_Handler:  1:  j 1b
Ecall_M_Mode_Handler:  1:  j 1b
Ecall_U_Mode_Handler:  1:  j 1b
Break_Point_Handler:  1:  j 1b
SysTick_Handler:  1:  j 1b
SW_Handler:  1:  j 1b
WWDG_IRQHandler:  1:  j 1b
PVD_IRQHandler:  1:  j 1b
TAMPER_IRQHandler:  1:  j 1b
RTC_IRQHandler:  1:  j 1b
FLASH_IRQHandler:  1:  j 1b
RCC_IRQHandler:  1:  j 1b
EXTI0_IRQHandler:  1:  j 1b
EXTI1_IRQHandler:  1:  j 1b
EXTI2_IRQHandler:  1:  j 1b
EXTI3_IRQHandler:  1:  j 1b
EXTI4_IRQHandler:  1:  j 1b
DMA1_Channel1_IRQHandler:  1:  j 1b
DMA1_Channel2_IRQHandler:  1:  j 1b
DMA1_Channel3_IRQHandler:  1:  j 1b
DMA1_Channel4_IRQHandler:  1:  j 1b
DMA1_Channel5_IRQHandler:  1:  j 1b
DMA1_Channel6_IRQHandler:  1:  j 1b
DMA1_Channel7_IRQHandler:  1:  j 1b
ADC1_2_IRQHandler:  1:  j 1b
USB_HP_CAN1_TX_IRQHandler:  1:  j 1b
USB_LP_CAN1_RX0_IRQHandler:  1:  j 1b
CAN1_RX1_IRQHandler:  1:  j 1b
CAN1_SCE_IRQHandler:  1:  j 1b
EXTI9_5_IRQHandler:  1:  j 1b
TIM1_BRK_IRQHandler:  1:  j 1b
TIM1_UP_IRQHandler:  1:  j 1b
TIM1_TRG_COM_IRQHandler:  1:  j 1b
TIM1_CC_IRQHandler:  1:  j 1b
TIM2_IRQHandler:  1:  j 1b
TIM3_IRQHandler:  1:  j 1b
TIM4_IRQHandler:  1:  j 1b
I2C1_EV_IRQHandler:  1:  j 1b
I2C1_ER_IRQHandler:  1:  j 1b
I2C2_EV_IRQHandler:  1:  j 1b
I2C2_ER_IRQHandler:  1:  j 1b
SPI1_IRQHandler:  1:  j 1b
SPI2_IRQHandler:  1:  j 1b
#USART1_IRQHandler:  1:  j 1b
USART2_IRQHandler:  1:  j 1b
USART3_IRQHandler:  1:  j 1b
EXTI15_10_IRQHandler:  1:  j 1b
RTCAlarm_IRQHandler:  1:  j 1b
TIM8_BRK_IRQHandler:  1:  j 1b
TIM8_UP_IRQHandler:  1:  j 1b
TIM8_TRG_COM_IRQHandler:  1:  j 1b
TIM8_CC_IRQHandler:  1:  j 1b
RNG_IRQHandler:  1:  j 1b
FSMC_IRQHandler:  1:  j 1b
SDIO_IRQHandler:  1:  j 1b
TIM5_IRQHandler:  1:  j 1b
SPI3_IRQHandler:  1:  j 1b
UART4_IRQHandler:  1:  j 1b
UART5_IRQHandler:  1:  j 1b
TIM6_IRQHandler:  1:  j 1b
TIM7_IRQHandler:  1:  j 1b
DMA2_Channel1_IRQHandler:  1:  j 1b
DMA2_Channel2_IRQHandler:  1:  j 1b
DMA2_Channel3_IRQHandler:  1:  j 1b
DMA2_Channel4_IRQHandler:  1:  j 1b
DMA2_Channel5_IRQHandler:  1:  j 1b
OTG_FS_IRQHandler:  1:  j 1b
UART6_IRQHandler:  1:  j 1b
UART7_IRQHandler:  1:  j 1b
UART8_IRQHandler:  1:  j 1b
TIM9_BRK_IRQHandler:  1:  j 1b
TIM9_UP_IRQHandler:  1:  j 1b
TIM9_TRG_COM_IRQHandler:  1:  j 1b
TIM9_CC_IRQHandler:  1:  j 1b
TIM10_BRK_IRQHandler:  1:  j 1b
TIM10_UP_IRQHandler:  1:  j 1b
TIM10_TRG_COM_IRQHandler:  1:  j 1b
TIM10_CC_IRQHandler:  1:  j 1b
DMA2_Channel6_IRQHandler:  1:  j 1b
DMA2_Channel7_IRQHandler:  1:  j 1b
DMA2_Channel8_IRQHandler:  1:  j 1b
DMA2_Channel9_IRQHandler:  1:  j 1b
DMA2_Channel10_IRQHandler:  1:  j 1b
DMA2_Channel11_IRQHandler:  1:  j 1b


.section	.text.handle_reset,"ax",@progbits
.global handle_reset
.text
handle_reset:
.option push
.option	norelax
	la gp, __global_pointer$
.option	pop

  la sp, _eusrstack

  la t0, _data_lma
  la t1, _data_vma
  la t2, _edata
  
COPY_DATA_LOOP:
  bgeu t1, t2, COPY_DATA_END
  lw t3, 0(t0)
  sw t3, 0(t1)
  addi t0, t0, 4
  addi t1, t1, 4
  j COPY_DATA_LOOP
COPY_DATA_END:

  la t0, _sbss
  la t1, _ebss
CLEAR_BSS_LOOP:
  bgeu t0, t1, CLEAR_BSS_END
  sw zero, 0(t0)
  addi t0, t0, 4
  j CLEAR_BSS_LOOP
CLEAR_BSS_END:



### Настройка адреса таблицы прыжков
  la t0, _vector_base
  ori t0, t0, 0b11   
  csrw mtvec, t0


  li t0, RCC_APB2ENR
  lw t1, 0(t0)
    li t2, RCC_APB2ENR_PAEN | RCC_APB2ENR_PBEN | RCC_APB2ENR_AFEN | RCC_APB2ENR_USART1EN
    or t1, t1, t2
  sw t1, 0(t0)
  
  li t0, GPIOA
  lw t1, GPIO_CFGHR(t0)
    li t2, ~((0b1111<<(4*(USART_RX-8))) | (0b1111<<(4*(USART_TX-8))) )
    and t1, t1, t2
    li t2, ((GPIO_INP<<(4*(USART_RX-8))) | (GPIO_ALT<<(4*(USART_TX-8))) )
    or t1, t1, t2
  sw t1, GPIO_CFGHR(t0)
  
  li t0, GPIOB
  lw t1, GPIO_CFGLR(t0)
    li t2, ~( (0b1111 << (4*RLED)) )
    and t1, t1, t2
    li t2, (GPIO_OUT << (4*RLED))
    or t1, t1, t2
  sw t1, GPIO_CFGLR(t0)

  li s11, USART1
  li t0, 8000000 / 9600
    sw t0, USART_BRR(s11)
  li t0, USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE
    sw t0, USART_CTLR1(s11)
  
  uart_puts_P "\r\n" __TIME__ ", " __DATE__ "\r\n"
  
  
/*******************************************************************************
 *******************************************************************************
 *******************************************************************************
 ***                                   *****************************************
 *** Начало осмысленного кода здесь    *****************************************
 ***                                   *****************************************
 *******************************************************************************
 *******************************************************************************
 *******************************************************************************/
  
#if 1 //# NA4
  li t0, (0x20004000 >> 2)
  csrw pmpaddr0, t0
  
  li t0, (((0b10<<3) | (0b000<<0))<<0)
  csrw pmpcfg0, t0
  uart_puts_P "PMP NA4 (4-byte page)\r\n"
#elif 0 //# TOP
  li t0, (0x20004000 >> 2)
  csrw pmpaddr0, t0
  li t0, (0x20004008 >> 2)
  csrw pmpaddr1, t0
  
  li t0, (((0b01<<3) | (0b000<<0))<<8)  |  (((0b00<<3) | (0b000<<0))<<0)
  csrw pmpcfg0, t0
  uart_puts_P "PMP range\r\n"
#elif 0 //# NAPOT
  #define PMP_PAGE(addr, pwr2) ( ( (addr &~ ((1<<pwr2)-1)) | ((1<<(pwr2-1))-1) )>>2 )
  li t0, PMP_PAGE(0x20004000, 3) # 2^3 = 8
  csrw pmpaddr0, t0
  
  li t0, (((0b11<<3) | (0b000<<0))<<0)
  csrw pmpcfg0, t0
  uart_puts_P "PMP variable size page (now: 8 bytes)\r\n"
#elif 1 //# intersection of areas
  #define PMP_PAGE(addr, pwr2) ( ( (addr &~ ((1<<pwr2)-1)) | ((1<<(pwr2-1))-1) )>>2 )
  li t0, (0x20004004 >> 2)
  csrw pmpaddr0, t0
  
  li t0, PMP_PAGE(0x20004000, 4) # 2^4 = 16
  csrw pmpaddr1, t0
  
  li t0,  (((0b11<<3) | (0b000<<0))<<8) | (((0b10<<3) | (0b111<<0))<<0)
  csrw pmpcfg0, t0
  uart_puts_P "PMP intersection\r\n"
#else
  #error Select one of PMP modes
#endif
  
  uart_puts_P "\r\nM-mode\r\n"
    
  uart_puts_P "ecall: "
  li a0,0xAAAAAAAA
  ecall
  
  uart_puts_P "csrr mstatus: "
  csrr t0, mstatus
  uart_puts_P "\r\n"
  
  uart_puts_P "lw 0x20002000: "
  li t0, 0x20002000
  lw t0, 0(t0)
  uart_puts_P "\r\n"
  
  uart_puts_P "lw 0x20004000: "
  li t0, 0x20004000
  lw t0, 0(t0)
  uart_puts_P "\r\n"
  
  
  li t0, MPP_U | MPIE | MIE
  csrw mstatus, t0
  la t0, main
  csrw mepc, t0
  mret
  
#a0...a1 - mem range
memtest:
  push s0, s1, s2
  mv s0, a0
  mv s1, a1
  mv s2, ra
memtest_loop:
    bgeu s0, s1, memtest_end
  mv a0, s0
  jal uart_putx
  lb t0, 0(s0)
  uart_puts_P "\r\n"
  addi s0 ,s0, 1
    j memtest_loop
memtest_end:
  mv ra, s2
  pop s0, s1, s2
ret
  
.global main
main:
  uart_puts_P "\r\nU-mode:\r\n"
  
  uart_puts_P "ecall: "
  li a0, 0x12345678
  ecall
  
  uart_puts_P "csrr mstatus: "
  csrr t0, mstatus
  uart_puts_P "\r\n"
  
  uart_puts_P "reading memory: \r\n"
  li a0, 0x20003FFE
  li a1, 0x20004012
  jal memtest

MAIN_LOOP:
  li t0, GPIOB
  lw t1, GPIO_OUTDR(t0)
    li t2, (1<<RLED)
    xor t1, t1, t2
  sw t1, GPIO_OUTDR(t0)
  
  
  li a0, 1000000
  call sleep
  
  j MAIN_LOOP

sleep:
  addi a0, a0, -1
  bnez a0, sleep
ret

### Обработчик исключительных ситуаций
.text
.align 2
trap_ecall_M:
  push t0, t1, a0, ra
  
  uart_puts_P "<--- ecall_M\r\n"
    
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  
  pop t0, t1, a0, ra
mret

trap_ecall_U:
  push t0, t1, a0, ra
  
  uart_puts_P "<--- ecall_U\r\n"
    
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  
  pop t0, t1, a0, ra
mret

trap_hardfault:
#mepc - код ошибки
  push t0, t1, a0, ra
  
  uart_puts_P " HardFault"
    
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  
  pop t0, t1, a0, ra
mret






uart_puts:
  li t0, USART1
UART_PUTS_WAIT:
  lw t1, USART_STATR(t0)
  andi t1, t1, USART_STATR_TXE
    beqz t1, UART_PUTS_WAIT
  lb t1, 0(a0)
  addi a0, a0, 1
    beqz t1, UART_PUTS_END
  sb t1, USART_DATAR(t0)
  j UART_PUTS_WAIT
UART_PUTS_END:
ret

.data
.section .bss
UART_PUT_BUF: .space 9
.text
uart_putx:
  la t0, UART_PUT_BUF
  sb zero, 8(t0)
  li t1, 28
UART_PUTX_LOOP:
  srl t2, a0, t1
  andi t2, t2, 0xF
  addi t3, t2, -10
    bltz t3, UART_PUTX_09
  addi t2, t3, 'A'-'0'
UART_PUTX_09:
  addi t2, t2, '0'
  sb t2, 0(t0)
  addi t0, t0, 1
  addi t1, t1, -4
    bgez t1, UART_PUTX_LOOP
  la a0, UART_PUT_BUF
j uart_puts  
