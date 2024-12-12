.equ PFIC_BASE,		0xE000E000
.equ PFIC_IENR1,	0x100

.equ USART1_IRQn,	53

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
    j   _start
    j   0
    j   NMI_Handler                /* NMI */
    j   HardFault_Handler          /* Hard Fault */
    j   0
#    .word   Ecall_M_Mode_Handler       /* Ecall M Mode */
j trap_entry
    j   0
    j   0
    j   Ecall_U_Mode_Handler       /* Ecall U Mode */
    j   Break_Point_Handler        /* Break Point */
    j   0
    j   0
    j   SysTick_Handler            /* SysTick */
    j   0
    j   SW_Handler                 /* SW */
    j   0
    /* External Interrupts */
    j   WWDG_IRQHandler            /* Window Watchdog */
    j   PVD_IRQHandler             /* PVD through EXTI Line detect */
    j   TAMPER_IRQHandler          /* TAMPER */
    j   RTC_IRQHandler             /* RTC */
    j   FLASH_IRQHandler           /* Flash */
    j   RCC_IRQHandler             /* RCC */
    j   EXTI0_IRQHandler           /* EXTI Line 0 */
    j   EXTI1_IRQHandler           /* EXTI Line 1 */
    j   EXTI2_IRQHandler           /* EXTI Line 2 */
    j   EXTI3_IRQHandler           /* EXTI Line 3 */
    j   EXTI4_IRQHandler           /* EXTI Line 4 */
    j   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
    j   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
    j   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
    j   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
    j   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
    j   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
    j   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
    j   ADC1_2_IRQHandler          /* ADC1_2 */
    j   USB_HP_CAN1_TX_IRQHandler  /* USB HP and CAN1 TX */
    j   USB_LP_CAN1_RX0_IRQHandler /* USB LP and CAN1RX0 */
    j   CAN1_RX1_IRQHandler        /* CAN1 RX1 */
    j   CAN1_SCE_IRQHandler        /* CAN1 SCE */
    j   EXTI9_5_IRQHandler         /* EXTI Line 9..5 */
    j   TIM1_BRK_IRQHandler        /* TIM1 Break */
    j   TIM1_UP_IRQHandler         /* TIM1 Update */
    j   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
    j   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
    j   TIM2_IRQHandler            /* TIM2 */
    j   TIM3_IRQHandler            /* TIM3 */
    j   TIM4_IRQHandler            /* TIM4 */
    j   I2C1_EV_IRQHandler         /* I2C1 Event */
    j   I2C1_ER_IRQHandler         /* I2C1 Error */
    j   I2C2_EV_IRQHandler         /* I2C2 Event */
    j   I2C2_ER_IRQHandler         /* I2C2 Error */
    j   SPI1_IRQHandler            /* SPI1 */
    j   SPI2_IRQHandler            /* SPI2 */
j   USART1_IRQHandler          /* USART1 */
    j   USART2_IRQHandler          /* USART2 */
    j   USART3_IRQHandler          /* USART3 */
    j   EXTI15_10_IRQHandler       /* EXTI Line 15..10 */
    j   RTCAlarm_IRQHandler        /* RTC Alarm through EXTI Line */
    j   0
    j   TIM8_BRK_IRQHandler        /* TIM8 Break */
    j   TIM8_UP_IRQHandler         /* TIM8 Update */
    j   TIM8_TRG_COM_IRQHandler    /* TIM8 Trigger and Commutation */
    j   TIM8_CC_IRQHandler         /* TIM8 Capture Compare */
    j   RNG_IRQHandler             /* RNG */
    j   FSMC_IRQHandler            /* FSMC */
    j   SDIO_IRQHandler            /* SDIO */
    j   TIM5_IRQHandler            /* TIM5 */
    j   SPI3_IRQHandler            /* SPI3 */
    j   UART4_IRQHandler           /* UART4 */
    j   UART5_IRQHandler           /* UART5 */
    j   TIM6_IRQHandler            /* TIM6 */
    j   TIM7_IRQHandler            /* TIM7 */
    j   DMA2_Channel1_IRQHandler   /* DMA2 Channel 1 */
    j   DMA2_Channel2_IRQHandler   /* DMA2 Channel 2 */
    j   DMA2_Channel3_IRQHandler   /* DMA2 Channel 3 */
    j   DMA2_Channel4_IRQHandler   /* DMA2 Channel 4 */
    j   DMA2_Channel5_IRQHandler   /* DMA2 Channel 5 */
    j   0
    j   0
    j   0
    j   0
    j   0
    j   0
    j   OTG_FS_IRQHandler          /* OTGFS */
    j   0
    j   0
    j   0
    j   UART6_IRQHandler           /* UART6 */
    j   UART7_IRQHandler           /* UART7 */
    j   UART8_IRQHandler           /* UART8 */
    j   TIM9_BRK_IRQHandler        /* TIM9 Break */
    j   TIM9_UP_IRQHandler         /* TIM9 Update */
    j   TIM9_TRG_COM_IRQHandler    /* TIM9 Trigger and Commutation */
    j   TIM9_CC_IRQHandler         /* TIM9 Capture Compare */
    j   TIM10_BRK_IRQHandler       /* TIM10 Break */
    j   TIM10_UP_IRQHandler        /* TIM10 Update */
    j   TIM10_TRG_COM_IRQHandler   /* TIM10 Trigger and Commutation */
    j   TIM10_CC_IRQHandler        /* TIM10 Capture Compare */
    j   DMA2_Channel6_IRQHandler   /* DMA2 Channel 6 */
    j   DMA2_Channel7_IRQHandler   /* DMA2 Channel 7 */
    j   DMA2_Channel8_IRQHandler   /* DMA2 Channel 8 */
    j   DMA2_Channel9_IRQHandler   /* DMA2 Channel 9 */
    j   DMA2_Channel10_IRQHandler  /* DMA2 Channel 10 */
    j   DMA2_Channel11_IRQHandler  /* DMA2 Channel 11 */

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
#    .weak   USART1_IRQHandler          /* USART1 */
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


.global handle_reset
.text
handle_reset:
  la sp, _stack_end

  la t0, _data_load
  la t1, _data_start
  la t2, _data_end
  
COPY_DATA_LOOP:
  bgeu t1, t2, COPY_DATA_END
  lw t3, 0(t0)
  sw t3, 0(t1)
  addi t0, t0, 4
  addi t1, t1, 4
  j COPY_DATA_LOOP
COPY_DATA_END:

  la t0, _bss_start
  la t1, _bss_end
CLEAR_BSS_LOOP:
  bgeu t0, t1, CLEAR_BSS_END
  sw zero, 0(t0)
  addi t0, t0, 4
  j CLEAR_BSS_LOOP
CLEAR_BSS_END:

### Настройка адреса таблицы прыжков
  la t0, _vector_base
  ori t0, t0, 0b01   
  csrw mtvec, t0
  
### Глобальное разрешение прерываний
  csrs mstatus, (1<<3) # MIE


  li t0, RCC_APB2ENR
  lw t1, 0(t0)
    li t2, RCC_APB2ENR_PAEN | RCC_APB2ENR_PBEN | RCC_APB2ENR_AFEN | RCC_APB2ENR_USART1EN
    or t1, t1, t2
  sw t1, 0(t0)
  
  li t0, GPIOA
  lw t1, GPIO_CFGHR(t0)
    li t2, ~((0b1111<<(4*(USART_RX-8))) | (0b1111<<(4*(USART_TX-8))) | (0b1111<<(4*(GLED-8))) )
    and t1, t1, t2
    li t2, ((GPIO_INP<<(4*(USART_RX-8))) | (GPIO_ALT<<(4*(USART_TX-8))) | (GPIO_OUT<<(4*(GLED-8))) )
    or t1, t1, t2
  sw t1, GPIO_CFGHR(t0)
  
  li t0, GPIOB
  lw t1, GPIO_CFGLR(t0)
    li t2, ~(0b1111 << (4*RLED))
    and t1, t1, t2
    li t2, (GPIO_OUT << (4*RLED))
    or t1, t1, t2
  sw t1, GPIO_CFGLR(t0)

  li s11, USART1
  li t0, 8000000 / 9600
    sw t0, USART_BRR(s11)
### TXEIE - флаг разрешения прерывания UART на передачу
  li t0, USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE  | USART_CTLR1_TXEIE
    sw t0, USART_CTLR1(s11)
  
  uart_puts_P "\r\n" __TIME__ ", " __DATE__ ". Hello from .rodata\r\n"
    
### Разрешаем прерывание UART в PFIC
  li t0, PFIC_BASE
  li t1, (1<<(USART1_IRQn & 31))
  sw t1, (PFIC_IENR1+4)(t0) # адрес 53 попадает в диапазон [32 ... 63], то есть второй регистр массива
  
  ecall
  
  li t5, 0
MAIN_LOOP:
  li t0, GPIOB
  lw t1, GPIO_OUTDR(t0)
    li t2, (1<<RLED)
    xor t1, t1, t2
  sw t1, GPIO_OUTDR(t0)
  
  li t0, GPIOA
  sw t5, GPIO_BSHR(t0)
  
  li t0, USART1
  li t1, USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE | USART_CTLR1_TXEIE
    sw t1, USART_CTLR1(t0)
  
  li a0, 5000000
  call sleep
  
  j MAIN_LOOP

sleep:
  addi a0, a0, -1
  bnez a0, sleep
ret

### Обработчик исключительных ситуаций
.text
.align 2
trap_entry:
  push t0, t1, t2, t3, a0, ra
  
  call uart_putx
  uart_puts_P "\r\n"
    
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  
  pop t0, t1, t2, t3, a0, ra
mret
  
USART1_IRQHandler:
  push t0, t1
  
  bnez t5, gled_off
gled_on:
  li t5, (1<<GLED)
  j gled_end
gled_off:
  slli t5, t5, 16
gled_end:
  
  li t0, USART1
  li t1, USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE
    sw t1, USART_CTLR1(t0)
  li t1, '+'
    sw t1, USART_DATAR(t0)
  pop t0, t1
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


.section .rodata
RO_STR: .asciz "\r\n" __TIME__ ", " __DATE__ ". Hello from .rodata\r\n"
NEWLNE: .asciz "\r\n"
INTR_STR: .asciz "Interrupt: "
