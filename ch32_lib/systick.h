#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#define SysTick_CTLR_STE (1<<0) //systick enable
#define SysTick_CTLR_STIE (1<<1) //interrupt enable
#define SysTick_CTLR_STCLK (1<<2) //clock source: 0-HCLK/8, 1-HCLK/1
#define SysTick_CTLR_STRE (1<<3) //auto reload enable
#define SysTick_CTLR_MODE (1<<4) //up/down. 0-up, 1-down
#define SysTick_CTLR_INIT (1<<31) //1: Updated to 0 when upcounting, while updated to the comparison value when downcounting.

#define SysTick_SR_CNTIF (1<<0) //interrupt flag

#if 0
__attribute__((interrupt))void SysTick_Handler(void){
  GPO_T(GLED);
  SysTick->SR=0;
}

  SysTick->SR=0;
  SysTick->CNT=0;
  SysTick->CMP = 8000000;
  
  SysTick->CTLR = SysTick_CTLR_STRE | SysTick_CTLR_STCLK | SysTick_CTLR_STIE | SysTick_CTLR_STE;
  
  NVIC_EnableIRQ(SysTicK_IRQn);
#endif

#endif
