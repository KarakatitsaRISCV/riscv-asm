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


.equ task_size, 1024
.equ task_count_max, 10

.data
task_count: .byte 0
task_idx: .byte 0

.bss
.align 2
local_stack: .space (task_size*task_count_max)
local_stack_en: .space 1

.global task_create
.text
task_create:
  mv t6, sp
  
  la t0, task_count
  lb t1, 0(t0)
  li t2, task_size
  mul t2, t2, t1
  la t3, local_stack_en
  sub t3, t3, t2
  addi sp, t3, -4
  //сохраняем все регистры кроме zero, ra, sp. С zero понятно, а ra, sp сохранятюся явно.
  push a0, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
  //ВАЖНО! Поскольку переключение будет в прерывании, mepc тоже надо сохранить
  push a0
  sw sp, 0(t3)
  
  addi t1, t1, 1
  sw t1, 0(t0)
  
  mv sp, t6
ret

.equ TIMER_CTRL_ADDR,   0xD1000000
.equ MTIME_LO,		0x00
.equ MTIME_HI,		0x04
.equ MTIMECMP_LO,	0x08
.equ MTIMECMP_HI,	0x0C

.equ CLIC_INT_TMR, 7

.equ GPIOB,  0x40010C00
.equ GPIO_OCTL,  0x0C

.global eclic_mtip_handler
.text
eclic_mtip_handler:
  push ra, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
  csrr t0, mepc
  push t0
  
  li t0, TIMER_CTRL_ADDR
  sw zero, MTIME_LO(t0)
  sw zero, MTIME_HI(t0)
  
//Декоративный элемент: мигает при каждом переключении контекста
  li t0, GPIOB
  lw t1, GPIO_OCTL(t0)
  xori t1, t1, (1<<6)
  sw t1, GPIO_OCTL(t0)
  
//save current stack
  la s0, task_count
  lb s1, 1(s0)
  li s2, task_size
  mul s3, s2, s1
  la s4, local_stack_en
  sub s5, s4, s3
  sw sp, 0(s5)
  
//next task
  addi s1, s1, 1
  lb s6, 0(s0)
  blt s1, s6, mtip_skip_wrap
    li s1, 0
  mtip_skip_wrap:
  sb s1, 1(s0)

//load next stack
  mul s3, s2, s1
  sub s5, s4, s3
  lw sp, 0(s5)
  
  pop t0
  csrw mepc, t0
  pop ra, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
mret

.global os_start
.text
os_start:
  li t0, TIMER_CTRL_ADDR
  sw zero, MTIMECMP_HI(t0)
  lw t1, SystemCoreClock
    li t2, 4000
    //div t1, t1, t2  // Чтобы получить частоту переключения примерно 1 kHz
  sw t1, MTIMECMP_LO(t0)
  sw zero, MTIME_LO(t0)
  sw zero, MTIME_HI(t0)
  
  li a0, CLIC_INT_TMR
  call eclic_set_vmode
  li a0, CLIC_INT_TMR
  call eclic_enable_interrupt

  la t2, local_stack_en
  lw sp, 0(t2)
  pop ra
  pop ra, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22 ,x23, x24, x25, x26, x27, x28, x29, x30, x31
ret