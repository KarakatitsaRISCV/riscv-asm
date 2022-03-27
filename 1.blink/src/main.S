#CTL=00, MD=11, OCTL=0/1
#CTL=01, MD=00
.equ PORTB_BASE,  0x40010C00
.equ PORTB_CTL0,  (PORTB_BASE + 0x00)
.equ PORTB_CTL1,  (PORTB_BASE + 0x04)
.equ PORTB_ISTAT, (PORTB_BASE + 0x08)
.equ PORTB_OCTL,  (PORTB_BASE + 0x0C)
.equ GPIO_OUT,  0b0011
.equ GPIO_INP,  0b0100

.equ RCU_APB2ENR_PAEN, (1<<2)
.equ RCU_APB2ENR_PBEN, (1<<3)
.equ RCU_APB2ENR, 0x40021018

.equ LED, 5
.equ LED2, 15
.equ BTN, 0

.global _start
.text
_start:
  li t0, RCU_APB2ENR
  lw t1, 0(t0)
    ori t1, t1, RCU_APB2ENR_PBEN
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
    
MAIN_LOOP:
  li t0, PORTB_OCTL
  lw t1, 0(t0)
    xori t1, t1, (1<<LED)
  sw t1, 0(t0)
  
  li t0, PORTB_ISTAT
  lw t1, 0(t0)
  andi t1, t1, (1<<BTN)
    bnez t1, SKIP_LED
  
  li t0, PORTB_OCTL
  lw t1, 0(t0)
    li t2, (1<<LED2)
    xor t1, t1, t2
  sw t1, 0(t0)
  
SKIP_LED:
  
  li t0, 500000
sleep:
  addi t0, t0, -1
  bnez t0, sleep
  
  j MAIN_LOOP
