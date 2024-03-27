#define GPIOA			(0x40010800)
#define GPIOB			(0x40010C00)
#define GPIO_CFGLR(PORT)	(PORT + 0x00)
#define GPIO_CFGHR(PORT)	(PORT + 0x04)
#define GPIO_INDR(PORT)		(PORT + 0x08)
#define GPIO_OUTDR(PORT)	(PORT + 0x0C)

.equ GPIO_PP50,  0b0011
.equ GPIO_HIZ,  0b0100
.equ GPIO_ALT,	0b1011

.equ RCC_IOPAEN, (1<<2)
.equ RCC_IOPBEN, (1<<3)
.equ RCC_IOPCEN, (1<<4)
.equ RCC_APB2PCENR, 0x40021018

.equ GLED,	11 # PA11
.equ RLED,	4  # PB4


.text
.global _start
_start:
  li sp, (0x20000000 + 32768)

  li t0, RCC_APB2PCENR
  lw t1, 0(t0)
    ori t1, t1, RCC_IOPAEN | RCC_IOPBEN;
  sw t1, 0(t0)
  
  li t0, GPIO_CFGLR( GPIOB )
  lw t1, 0(t0)
    li t2, ~((0b1111 << (4*RLED)) )
    and t1, t1, t2
    li t2, (GPIO_PP50 << (4*RLED))
    or t1, t1, t2
  sw t1, 0(t0)
  
  li s1, GPIO_OUTDR( GPIOB )
  li s2, (1<<RLED)
  

loop:
  lw t0, 0(s1)
  xor t0, t0, s2
  sw t0, 0(s1)
  
  li a0, 1000000
  call sleep
    j loop
  
  
  
sleep:
  addi a0, a0, -1
  bnez a0, sleep
ret