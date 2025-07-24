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

#CH32-specific CSR
.equ INTSYSCR, 0x804
.equ GIHWSTKNEN, (1<<5)

.equ GPIOB,  0x40010C00
.equ GPIO_OCTL,  0x0C

.equ SYSTICK, 0xE000F000
.equ SYSTICK_CTLR,	0
.equ SYSTICK_SR,	4
.equ SYSTICK_CNTL,	8
.equ SYSTICK_CNTH,	12
.equ SYSTICK_CMPL,	16
.equ SYSTICK_CMPH,	20
.equ SysTick_CTLR_STCLK,	(1<<2) #clock source: 0-HCLK/8, 1-HCLK/1
.equ SysTick_CTLR_STIE,		(1<<1) #interrupt enable
.equ SysTick_CTLR_STE,		(1<<0) #systick enable
.equ SysTicK_IRQn, 12 #12 System timer Interrupt

.equ NVIC, 0xE000E000
.equ NVIC_IENR0, 256
.equ NVIC_IPRR0, 640

.equ OS_DT, 8000000


.equ task_size, 1024
.equ task_count_max, 10

.data
task_count: .byte 0
task_idx: .byte 0

.bss
.align 4
local_stack: .space (task_size*task_count_max)
local_stack_en: .space 1

.global task_create
.text
task_create:
  mv t6, sp
  
  la t0, task_count
  lbu t1, 0(t0)
  li t2, task_size
  mul t2, t2, t1
  la t3, local_stack_en
  sub t3, t3, t2
  addi sp, t3, -4
  #сохраняем все регистры кроме zero, ra, sp. С zero понятно, а ra, sp сохранятюся явно.
  push a0, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
  #ВАЖНО! Поскольку переключение будет в прерывании, mepc тоже надо сохранить
  push a0
  sw sp, 0(t3)
  
  addi t1, t1, 1
  sb t1, 0(t0)
  
  mv sp, t6
ret

.global SysTick_Handler
SysTick_Handler:
  push ra, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
  csrr t0, mepc
  push t0
  
  li t0, SYSTICK
  lw t2, SYSTICK_CMPL(t0) # t2 = low
  lw t3, SYSTICK_CMPH(t0) # t3 = high
# CMP += OS_DT
  li t1, (OS_DT & 0xFFFFFFFF)
  add t1, t1, t2 # res_lo = OS_DT + low
  sltu t2, t1, t2 # if(res_lo < low) res_high = 1 | res_high=t2
  add t2, t2, t3 # res_high += high
  li t3, (OS_DT >> 32)
  add t2, t2, t3 # res_high += OS_DT
# write CMP
  sw t2, SYSTICK_CMPH(t0)
  sw t1, SYSTICK_CMPL(t0)
# Systick->SP = 0
  sw zero, SYSTICK_SR(t0)

#Декоративный элемент: мигает при каждом переключении контекста
  li t0, GPIOB
  lw t1, GPIO_OCTL(t0)
  xori t1, t1, (1<<4)
  sw t1, GPIO_OCTL(t0)
  
#save current stack
  la s0, task_count
  lbu s1, 1(s0)
  li s2, task_size
  mul s3, s2, s1
  la s4, local_stack_en
  sub s5, s4, s3
  sw sp, 0(s5)
  
#next task
  addi s1, s1, 1
  lbu s6, 0(s0)
  blt s1, s6, mtip_skip_wrap
    li s1, 0
  mtip_skip_wrap:
  sb s1, 1(s0)

#load next stack
  mul s3, s2, s1
  sub s5, s4, s3
  lw sp, 0(s5)

#CH32-specific; workaround WCH-interrupt-fast
  li t0, GIHWSTKNEN
  csrs INTSYSCR, t0
  
  pop t0
  csrw mepc, t0
  pop ra, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
mret



.global os_start
.text
os_start:
  li t0, SYSTICK
  sw zero, SYSTICK_CTLR(t0)
  li t1, (OS_DT >> 32)
  sw t1, SYSTICK_CMPH(t0)
  li t1, (OS_DT & 0xFFFFFFFF)
  sw t1, SYSTICK_CMPL(t0)
  sw zero, SYSTICK_CNTL(t0)
  sw zero, SYSTICK_CNTH(t0)
  li t1, SysTick_CTLR_STCLK | SysTick_CTLR_STIE | SysTick_CTLR_STE
  sw t1, SYSTICK_CTLR(t0)
  sw zero, SYSTICK_SR(t0)
  
  li t0, NVIC
  li t1, (1<<SysTicK_IRQn)
  sw t1, NVIC_IPRR0(t0) # NVIC_ClearPendingIRQ(SysTicK_IRQn);
  sw t1, NVIC_IENR0(t0) # NVIC_EnableIRQ(SysTicK_IRQn);
  
  la t2, local_stack_en
  lw sp, 0(t2)
  pop ra
  pop ra, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
ret