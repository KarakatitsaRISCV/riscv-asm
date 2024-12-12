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

### Настройка адреса обработчика прерывания
  la t0, trap_entry
  ori t0, t0, 0    
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
  
  li a0, 100
  call sleep
  
### Различные типы исключений:
  .word 0xFFFFFFFF # некорректная инструкция, mcause = 2
  ecall # ecall, mcause = 11 (0x0B)
  li t0, 0x20000001
  lw t1, 0(t0) # невыровненный адрес, mcause = 4

MAIN_LOOP:
  li t0, GPIOB
  lw t1, GPIO_OUTDR(t0)
    li t2, (1<<RLED)
    xor t1, t1, t2
  sw t1, GPIO_OUTDR(t0)
  
  li a0, 500000
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
  
  csrr a0, mcause
    bltz a0, irq_entry # Если выставлен 31-й бит, то поймали не исключение, а прерывание. Его обрабатываем отдельно
  
  call uart_putx
  uart_puts_P "\r\n"
  
  li t0, GPIOA
  li t1, (1<<GLED)
  sw t1, GPIO_BSHR(t0)
    
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  
TRAP_END:
  pop t0, t1, t2, t3, a0, ra
mret
  
irq_entry:
  li t0, USART1
  li t1, USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE
    sw t1, USART_CTLR1(t0)
  
  uart_puts_P "Interrupt: "
  csrr a0, mcause
  call uart_putx
  uart_puts_P "\r\n"
j TRAP_END

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
