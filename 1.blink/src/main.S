#CTL=00, MD=11, OCTL=0/1
.equ PORTA_BASE,  0x40010800
.equ PORTA_CTL0,  (PORTA_BASE + 0x00)
.equ PORTA_CTL1,  (PORTA_BASE + 0x04)
.equ PORTA_ISTAT, (PORTA_BASE + 0x08)
.equ PORTA_OCTL,  (PORTA_BASE + 0x0C)
.equ PORTB_BASE,  0x40010C00
.equ PORTB_CTL0,  (PORTB_BASE + 0x00)
.equ PORTB_CTL1,  (PORTB_BASE + 0x04)
.equ PORTB_ISTAT, (PORTB_BASE + 0x08)
.equ PORTB_OCTL,  (PORTB_BASE + 0x0C)
.equ GPIO_OUT,  0b0011

.equ RCU_APB2ENR_PAEN, (1<<2)
.equ RCU_APB2ENR_PBEN, (1<<3)
.equ RCU_APB2ENR, 0x40021018

.equ LED, 5
.equ LED2, 5

.global _start
.text
_start:
  li t0, RCU_APB2ENR
  lw t1, 0(t0)
    ori t1, t1, RCU_APB2ENR_PBEN | RCU_APB2ENR_PAEN
  sw t1, 0(t0)

  li t0, PORTB_CTL0
  lw t1, 0(t0)
    li t2, ~(0b1111 << (4*LED))
    and t1, t1, t2
    li t2, 0b0011 << (4*LED)
    or t1, t1, t2
  sw t1, 0(t0)
  
  li t0, PORTA_CTL0
  lw t1, 0(t0)
    li t2, ~(0b1111 << (4*LED2))
    and t1, t1, t2
    li t2, 0b0011 << (4*LED2)
    or t1, t1, t2
  sw t1, 0(t0)
  
MAIN_LOOP:
  li t0, PORTB_OCTL
  lw t1, 0(t0)
    xori t1, t1, (1<<LED)
  sw t1, 0(t0)
  
  li t0, PORTA_OCTL
  lw t1, 0(t0)
    xori t1, t1, (1<<LED2)
  sw t1, 0(t0)
  
  
  li t0, 500000
sleep:
  addi t0, t0, -1
  bnez t0, sleep
  
  j MAIN_LOOP
