.equ mtvt,		0x307
.equ mtvt2,		0x7EC
.equ mnvec,		0x7C3
.equ mmisc_ctl,		0x7D0

.text
.section .isr_vectors
.align 9
vector_base:
  j _start
  .align    2
  .word  0
  .word  0
  .word  eclic_msip_handler
  .word  0
  .word  0
  .word  0
  .word  eclic_mtip_handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  eclic_bwei_handler
  .word  eclic_pmovi_handler
  .word  WWDGT_IRQHandler
  .word  LVD_IRQHandler
  .word  TAMPER_IRQHandler
  .word  RTC_IRQHandler
  .word  FMC_IRQHandler
  .word  RCU_IRQHandler
  .word  EXTI0_IRQHandler
  .word  EXTI1_IRQHandler
  .word  EXTI2_IRQHandler
  .word  EXTI3_IRQHandler
  .word  EXTI4_IRQHandler
  .word  DMA0_Channel0_IRQHandler
  .word  DMA0_Channel1_IRQHandler
  .word  DMA0_Channel2_IRQHandler
  .word  DMA0_Channel3_IRQHandler
  .word  DMA0_Channel4_IRQHandler
  .word  DMA0_Channel5_IRQHandler
  .word  DMA0_Channel6_IRQHandler
  .word  ADC0_1_IRQHandler
  .word  CAN0_TX_IRQHandler
  .word  CAN0_RX0_IRQHandler
  .word  CAN0_RX1_IRQHandler
  .word  CAN0_EWMC_IRQHandler
  .word  EXTI5_9_IRQHandler
  .word  TIMER0_BRK_IRQHandler
  .word  TIMER0_UP_IRQHandler
  .word  TIMER0_TRG_CMT_IRQHandler
  .word  TIMER0_Channel_IRQHandler
  .word  TIMER1_IRQHandler
  .word  TIMER2_IRQHandler
  .word  TIMER3_IRQHandler
  .word  I2C0_EV_IRQHandler
  .word  I2C0_ER_IRQHandler
  .word  I2C1_EV_IRQHandler
  .word  I2C1_ER_IRQHandler
  .word  SPI0_IRQHandler
  .word  SPI1_IRQHandler
  .word  USART0_IRQHandler
  .word  USART1_IRQHandler
  .word  USART2_IRQHandler
  .word  EXTI10_15_IRQHandler
  .word  RTC_Alarm_IRQHandler
  .word  USBFS_WKUP_IRQHandler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  EXMC_IRQHandler
  .word  0
  .word  TIMER4_IRQHandler
  .word  SPI2_IRQHandler
  .word  UART3_IRQHandler
  .word  UART4_IRQHandler
  .word  TIMER5_IRQHandler
  .word  TIMER6_IRQHandler
  .word  DMA1_Channel0_IRQHandler
  .word  DMA1_Channel1_IRQHandler
  .word  DMA1_Channel2_IRQHandler
  .word  DMA1_Channel3_IRQHandler
  .word  DMA1_Channel4_IRQHandler
  .word  0
  .word  0
  .word  CAN1_TX_IRQHandler
  .word  CAN1_RX0_IRQHandler
  .word  CAN1_RX1_IRQHandler
  .word  CAN1_EWMC_IRQHandler
  .word  USBFS_IRQHandler


.global _start
.text
.section .start
_start:
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

  la t0, TrapHandler
    ori t0, t0, 0b000011
  csrw mtvec, t0
  
  la t0, IRQHandler
    ori t0, t0, 1
  csrw mtvt2, t0
  
  la t0, NMIHandler
  csrw mnvec, t0
  li t0, (1<<9) #0-reset, 1-interrupt
  csrs mmisc_ctl, t0
  
  la t0, vector_base
  csrw mtvt, t0
  
  li a0, 0
  li a1, 0
  j main

.text
.align 6
.weak TrapHandler
TrapHandler:
.weak IRQHandler
IRQHandler:
.weak NMIHandler
NMIHandler:
j UnhandledInterruptHandler

    .weak  eclic_msip_handler
    .weak  eclic_mtip_handler
    .weak  eclic_bwei_handler
    .weak  eclic_pmovi_handler
    .weak  WWDGT_IRQHandler
    .weak  LVD_IRQHandler
    .weak  TAMPER_IRQHandler
    .weak  RTC_IRQHandler
    .weak  FMC_IRQHandler
    .weak  RCU_IRQHandler
    .weak  EXTI0_IRQHandler
    .weak  EXTI1_IRQHandler
    .weak  EXTI2_IRQHandler
    .weak  EXTI3_IRQHandler
    .weak  EXTI4_IRQHandler
    .weak  DMA0_Channel0_IRQHandler
    .weak  DMA0_Channel1_IRQHandler
    .weak  DMA0_Channel2_IRQHandler
    .weak  DMA0_Channel3_IRQHandler
    .weak  DMA0_Channel4_IRQHandler
    .weak  DMA0_Channel5_IRQHandler
    .weak  DMA0_Channel6_IRQHandler
    .weak  ADC0_1_IRQHandler
    .weak  CAN0_TX_IRQHandler
    .weak  CAN0_RX0_IRQHandler
    .weak  CAN0_RX1_IRQHandler
    .weak  CAN0_EWMC_IRQHandler
    .weak  EXTI5_9_IRQHandler
    .weak  TIMER0_BRK_IRQHandler
    .weak  TIMER0_UP_IRQHandler
    .weak  TIMER0_TRG_CMT_IRQHandler
    .weak  TIMER0_Channel_IRQHandler
    .weak  TIMER1_IRQHandler
    .weak  TIMER2_IRQHandler
    .weak  TIMER3_IRQHandler
    .weak  I2C0_EV_IRQHandler
    .weak  I2C0_ER_IRQHandler
    .weak  I2C1_EV_IRQHandler
    .weak  I2C1_ER_IRQHandler
    .weak  SPI0_IRQHandler
    .weak  SPI1_IRQHandler
    .weak  USART0_IRQHandler
    .weak  USART1_IRQHandler
    .weak  USART2_IRQHandler
    .weak  EXTI10_15_IRQHandler
    .weak  RTC_Alarm_IRQHandler
    .weak  USBFS_WKUP_IRQHandler
    .weak  EXMC_IRQHandler
    .weak  TIMER4_IRQHandler
    .weak  SPI2_IRQHandler
    .weak  UART3_IRQHandler
    .weak  UART4_IRQHandler
    .weak  TIMER5_IRQHandler
    .weak  TIMER6_IRQHandler
    .weak  DMA1_Channel0_IRQHandler
    .weak  DMA1_Channel1_IRQHandler
    .weak  DMA1_Channel2_IRQHandler
    .weak  DMA1_Channel3_IRQHandler
    .weak  DMA1_Channel4_IRQHandler
    .weak  CAN1_TX_IRQHandler
    .weak  CAN1_RX0_IRQHandler
    .weak  CAN1_RX1_IRQHandler
    .weak  CAN1_EWMC_IRQHandler
    .weak  USBFS_IRQHandler
eclic_msip_handler:
eclic_mtip_handler:
eclic_bwei_handler:
eclic_pmovi_handler:
WWDGT_IRQHandler:
LVD_IRQHandler:
TAMPER_IRQHandler:
RTC_IRQHandler:
FMC_IRQHandler:
RCU_IRQHandler:
EXTI0_IRQHandler:
EXTI1_IRQHandler:
EXTI2_IRQHandler:
EXTI3_IRQHandler:
EXTI4_IRQHandler:
DMA0_Channel0_IRQHandler:
DMA0_Channel1_IRQHandler:
DMA0_Channel2_IRQHandler:
DMA0_Channel3_IRQHandler:
DMA0_Channel4_IRQHandler:
DMA0_Channel5_IRQHandler:
DMA0_Channel6_IRQHandler:
ADC0_1_IRQHandler:
CAN0_TX_IRQHandler:
CAN0_RX0_IRQHandler:
CAN0_RX1_IRQHandler:
CAN0_EWMC_IRQHandler:
EXTI5_9_IRQHandler:
TIMER0_BRK_IRQHandler:
TIMER0_UP_IRQHandler:
TIMER0_TRG_CMT_IRQHandler:
TIMER0_Channel_IRQHandler:
TIMER1_IRQHandler:
TIMER2_IRQHandler:
TIMER3_IRQHandler:
I2C0_EV_IRQHandler:
I2C0_ER_IRQHandler:
I2C1_EV_IRQHandler:
I2C1_ER_IRQHandler:
SPI0_IRQHandler:
SPI1_IRQHandler:
USART0_IRQHandler:
USART1_IRQHandler:
USART2_IRQHandler:
EXTI10_15_IRQHandler:
RTC_Alarm_IRQHandler:
USBFS_WKUP_IRQHandler:
EXMC_IRQHandler:
TIMER4_IRQHandler:
SPI2_IRQHandler:
UART3_IRQHandler:
UART4_IRQHandler:
TIMER5_IRQHandler:
TIMER6_IRQHandler:
DMA1_Channel0_IRQHandler:
DMA1_Channel1_IRQHandler:
DMA1_Channel2_IRQHandler:
DMA1_Channel3_IRQHandler:
DMA1_Channel4_IRQHandler:
CAN1_TX_IRQHandler:
CAN1_RX0_IRQHandler:
CAN1_RX1_IRQHandler:
CAN1_EWMC_IRQHandler:
USBFS_IRQHandler:

.weak UnhandledInterruptHandler
UnhandledInterruptHandler:
  j UnhandledInterruptHandler