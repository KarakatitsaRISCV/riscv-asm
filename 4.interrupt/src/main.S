.equ mtvt,				0x307
.equ mtvt2,				0x7EC
.equ MSTATUS_MIE, 		(1<<3)

.equ ECLIC_ADDR,			0xD2000000
.equ ECLIC_CLICINT,		(ECLIC_ADDR + 0x1000)
.equ ECLIC_CFG,				0x0
.equ ECLIC_INFO,			0x4
.equ ECLIC_MTH,				0xB
.equ ECLIC_CLICINTIP,	0
.equ ECLIC_CLICINTIE,	1
.equ ECLIC_CLICINTATTR,2
.equ ECLIC_CLICINTCTL,3

.equ USART0_IRQn,			56

.equ GPIOA,  0x40010800
.equ GPIOB,  0x40010C00
.equ GPIO_CTL0,  0x00
.equ GPIO_CTL1,  0x04
.equ GPIO_ISTAT, 0x08
.equ GPIO_OCTL,  0x0C
.equ GPIO_BOP,   0x10

.equ GPIO_OUT,  0b0011
.equ GPIO_INP,  0b0100
.equ GPIO_ALT,	0b1011

.equ USART0,			0x40013800
.equ USART_STAT,	0
.equ USART_DATA,	4
.equ USART_BAUD,	8
.equ USART_CTL0,	0x0C

.equ USART_STAT_TBE,	(1<<7)
.equ USART_STAT_RBNE,	(1<<5)
.equ USART_CTL0_UEN,	(1<<13)
.equ USART_CTL0_TEN,	(1<<3)
.equ USART_CTL0_REN,	(1<<2)
.equ USART_CTL0_TBEIE,(1<<7)

.equ RCU_APB2ENR_AFEN, (1<<0)
.equ RCU_APB2ENR_PAEN, (1<<2)
.equ RCU_APB2ENR_PBEN, (1<<3)
.equ RCU_APB2ENR_USART0EN,	(1<<14)
.equ RCU_APB2ENR, 0x40021018

#PORTA
.equ USART_TX,	9
.equ USART_RX,	10

.equ RLED, 5
.equ YLED, 6
.equ GLED, 7

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

.global _start
.text
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

  la t0, trap_entry
    ori t0, t0, 0b000011
  csrw mtvec, t0
  
  #la t0, irq_entry
  #  ori t0, t0, 1
  #csrw mtvt2, t0
  la t0, vector_base
  csrw mtvt, t0
  
  li t0, RCU_APB2ENR
  lw t1, 0(t0)
    li t2, RCU_APB2ENR_PAEN | RCU_APB2ENR_PBEN | RCU_APB2ENR_AFEN | RCU_APB2ENR_USART0EN
    or t1, t1, t2
  sw t1, 0(t0)
  
  li t0, GPIOA
  lw t1, GPIO_CTL1(t0)
    li t2, ~((0b1111<<(4*(USART_RX-8))) | (0b1111<<(4*(USART_TX-8))))
    and t1, t1, t2
    li t2, ((GPIO_INP<<(4*(USART_RX-8))) | (GPIO_ALT<<(4*(USART_TX-8))))
    or t1, t1, t2
  sw t1, GPIO_CTL1(t0)
  
  li t0, GPIOB
  lw t1, GPIO_CTL0(t0)
    li t2, ~((0b1111 << (4*RLED)) | (0b1111 << (4*YLED)) | (0b1111 << (4*GLED)))
    and t1, t1, t2
    li t2, ((GPIO_OUT << (4*RLED)) | (GPIO_OUT << (4*YLED)) | (GPIO_OUT << (4*GLED)))
    or t1, t1, t2
    
    li t2, ~(0b1111 << (4*BTN))
    and t1, t1, t2
    li t2, GPIO_INP << (4*BTN)
    or t1, t1, t2
  sw t1, GPIO_CTL0(t0)
  
  li s11, USART0
  li t0, 8000000 / 9600
    sw t0, USART_BAUD(s11)
  li t0, USART_CTL0_UEN | USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_TBEIE
    sw t0, USART_CTL0(s11)
  
  li t0, ECLIC_CLICINT
    li t1, 1
  sb t1, (ECLIC_CLICINTIE + 4*USART0_IRQn)(t0)
  sb t1, (ECLIC_CLICINTATTR+4*USART0_IRQn)(t0)
  
  
  la a0, RO_STR
  call uart_puts
  
  csrs mstatus, MSTATUS_MIE
  
  #.word 0xFFFFFFFF
  li a0, 0x321
  li a7, 1
  ecall
  
  la a0, ECALL_STR
  li a7, 4
  ecall
  
  ebreak
  
  li a0, 0x123
  call uart_putx
MAIN_LOOP:
  li t0, GPIOB
  lw t1, GPIO_OCTL(t0)
    li t2, (1<<YLED)
    xor t1, t1, t2
  sw t1, GPIO_OCTL(t0)
  
  li a0, 500000
  call sleep
  
  j MAIN_LOOP
  
sleep:
  addi a0, a0, -1
  bnez a0, sleep
ret

uart_puts:
  li t0, USART0
UART_PUTS_WAIT:
  lw t1, USART_STAT(t0)
  andi t1, t1, USART_STAT_TBE
    beqz t1, UART_PUTS_WAIT
  lb t1, 0(a0)
  addi a0, a0, 1
    beqz t1, UART_PUTS_END
  sb t1, USART_DATA(t0)
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
RO_STR: .asciz "Hello from .rodata\r\n"
ECALL_STR: .asciz "ecall\r\n"

.text
.section .isr_vectors
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

.text
.align 2
USART0_IRQHandler:
  push t0, t1
  
  li t0, GPIOB
  lh t1, GPIO_OCTL(t0)
    xori t1, t1, (1<<GLED)
  sh t1, GPIO_OCTL(t0)
  
  li t0, USART0
  li t1, USART_CTL0_REN | USART_CTL0_TEN | USART_CTL0_UEN
    sw t1, USART_CTL0(t0)
  li t1, '+'
    sw t1, USART_DATA(t0)
  
  pop t0, t1
mret

.text
.align 6
trap_entry:
  push t0, t1, ra
  
  csrr t0, mcause
  andi t0, t0, 0x7FF
  addi t1, t0, -11
    beqz t1, TRAP_ECALL
  addi t1, t0, -8
    beqz t1, TRAP_ECALL
  j TRAP_END
  
TRAP_ECALL:
  addi t0, a7, -1
    bnez t0, TRAP_SKIP_PUTI
  call uart_putx
  j TRAP_END
TRAP_SKIP_PUTI:
  addi t0, a7, -4
    bnez t0, TRAP_SKIP_PUTS
  call uart_puts
  j TRAP_END
TRAP_SKIP_PUTS:

TRAP_END:
  
  li t0, GPIOB
  lh t1, GPIO_OCTL(t0)
    xori t1, t1, (1<<RLED)
  sh t1, GPIO_OCTL(t0)
  
  csrr t0, mepc
  lh t1, 0(t0)
  andi t1, t1, 0b11
  addi t1, t1, -3
  bnez t1, TRAP_INSTR_COMPR
    addi t0, t0, 2
TRAP_INSTR_COMPR:
  addi t0, t0, 2
  
  csrw mepc, t0
  
  pop t0, t1, ra
mret
