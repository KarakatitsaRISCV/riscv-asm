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

.global _start
.text
_start:
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
  li t0, 'S'
    sw t0, USART_DATA(s11)
    
MAIN_LOOP:
  lw t0, USART_STAT(s11)
  andi t0, t0, USART_STAT_RBNE
    beqz t0, UART_SKIP
  lw t0, USART_DATA(s11)
  addi t0, t0, -'r'
  li t1, (1<<LED)
    beqz t0, LED_SET
  slli t1, t1, 16
LED_SET:
  li t0, PORTB_BOP
  sw t1, 0(t0)
  
  #li t0, PORTB_OCTL
  #lw t1, 0(t0)
  #  xori t1, t1, (1<<LED)
  #sw t1, 0(t0)
    
UART_SKIP:
  
  li t0, 500000
sleep:
  addi t0, t0, -1
  bnez t0, sleep
  
  j MAIN_LOOP
