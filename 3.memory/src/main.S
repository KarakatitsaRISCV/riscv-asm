#CTL=00, MD=11, OCTL=0/1
#CTL=01, MD=00
.equ PORTA_BASE,  0x40010800
.equ PORTA_CTL0,  (PORTA_BASE + 0x00)
.equ PORTA_CTL1,  (PORTA_BASE + 0x04)
.equ PORTA_ISTAT, (PORTA_BASE + 0x08)
.equ PORTA_OCTL,  (PORTA_BASE + 0x0C)
.equ PORTA_BOP,   (PORTA_BASE + 0x10)
.equ PORTB_BASE,  0x40010C00
.equ PORTB_CTL0,  (PORTB_BASE + 0x00)
.equ PORTB_CTL1,  (PORTB_BASE + 0x04)
.equ PORTB_ISTAT, (PORTB_BASE + 0x08)
.equ PORTB_OCTL,  (PORTB_BASE + 0x0C)
.equ PORTB_BOP,   (PORTB_BASE + 0x10)
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

.equ RCU_APB2ENR_AFEN, (1<<0)
.equ RCU_APB2ENR_PAEN, (1<<2)
.equ RCU_APB2ENR_PBEN, (1<<3)
.equ RCU_APB2ENR_USART0EN,	(1<<14)
.equ RCU_APB2ENR, 0x40021018

#PORTA
.equ USART_TX,	9
.equ USART_RX,	10

.equ LED, 5
.equ LED2, 15
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
  
  push s0, s1, s2, s3
  pop s0, s1, s2, s3

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

  li t0, RCU_APB2ENR
  lw t1, 0(t0)
    li t2, RCU_APB2ENR_PAEN | RCU_APB2ENR_PBEN | RCU_APB2ENR_AFEN | RCU_APB2ENR_USART0EN
    or t1, t1, t2
  sw t1, 0(t0)
  
  li t0, PORTA_CTL1
  lw t1, 0(t0)
    li t2, ~((0b1111<<(4*(USART_RX-8))) | (0b1111<<(4*(USART_TX-8))))
    and t1, t1, t2
    li t2, ((GPIO_INP<<(4*(USART_RX-8))) | (GPIO_ALT<<(4*(USART_TX-8))))
    or t1, t1, t2
  sw t1, 0(t0)
  
  li t0, PORTB_CTL0
  lw t1, 0(t0)
    li t2, ~(0b1111 << (4*LED))
    and t1, t1, t2
    li t2, 0b0011 << (4*LED)
    or t1, t1, t2
    
    li t2, ~(0b1111 << (4*BTN))
    and t1, t1, t2
    li t2, 0b0100 << (4*BTN)
    or t1, t1, t2
  sw t1, 0(t0)
  
  
  li t0, PORTB_CTL1
  lw t1, 0(t0)
    li t2, ~(0b1111 << (4*(LED2-8)))
    and t1, t1, t2
    li t2, 0b0011 << (4*(LED2-8))
    or t1, t1, t2
  sw t1, 0(t0)
  
  li s11, USART0
  li t0, 8000000 / 9600
    sw t0, USART_BAUD(s11)
  li t0, USART_CTL0_UEN | USART_CTL0_TEN | USART_CTL0_REN
    sw t0, USART_CTL0(s11)
  
  li a0, 0x89ABCDEF
  call uart_putx
  
  la a0, TEXT_STR
  call uart_puts
  
  la a0, DATA_STR
  call uart_puts
  
  la a0, RO_STR
  call uart_puts
  
MAIN_LOOP:
  
  li t0, 500000
sleep:
  addi t0, t0, -1
  bnez t0, sleep
  
  j MAIN_LOOP

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

.text
TEXT_STR: .asciz "Hello from .text\r\n"

.section .rodata
RO_STR: .asciz "Hello from .rodata\r\n"

.data
DATA_STR: .asciz "Hello from .data\r\n"
