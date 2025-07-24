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
  push a0, gp, tp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11
  sw sp, 0(t3)
  
  addi t1, t1, 1
  sw t1, 0(t0)
  
  mv sp, t6
ret

.global yield
.text
yield:
  push ra, gp, tp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11
  
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
  blt s1, s6, yield_skip_wrap
    li s1, 0
  yield_skip_wrap:
  sb s1, 1(s0)

//load next stack
  mul s3, s2, s1
  sub s5, s4, s3
  lw sp, 0(s5)
  
  pop ra, gp, tp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11
ret

.global os_start
.text
os_start:
  la t3, local_stack_en
  lw sp, 0(t3)
  pop ra, gp, tp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11
ret