#ifndef __V30x_CLOCK_H__
#define __V30x_CLOCK_H__

#ifndef CH32V30x_D8C
  //#error This clock.h is for ch32v305 and ch32v307
#endif

#define CLOCK_HSI 0
#define CLOCK_HSE 1

int8_t clock_HS(int8_t quartz){
  uint32_t timeout;
  RCC->CFGR0 |= RCC_HPRE_DIV1;
  RCC->CFGR0 |= RCC_PPRE2_DIV2;
  RCC->CFGR0 |= RCC_PPRE1_DIV2;
  RCC->CFGR0 &=~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL);

  //try to start HSE
  if(quartz){
    RCC->CTLR |= RCC_HSEON;
    timeout = 100000;
    while(--timeout){if(RCC->CTLR & RCC_HSERDY)break;}
    if(timeout != 0){
      //prediv1src = HSE, not PLL2
      //prediv1 = /1
      RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE);
    }else{
      quartz = 0;
    }
  }
  if(!quartz){
    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSI_Div2);
  }
  #ifdef CH32V30x_D8
    RCC->CFGR0 |= (uint32_t)(RCC_PLLMULL18);
  #else
    RCC->CFGR2 = 0;
    RCC->CFGR0 |= (uint32_t)(RCC_PLLMULL18_EXTEN);
  #endif
  
  RCC->CTLR |= RCC_PLLON;
  while(!(RCC->CTLR & RCC_PLLRDY)){}
  
  RCC->CFGR0 = (RCC->CFGR0 &~ RCC_SW) | RCC_SW_PLL;
  while((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL){}
  
  if(quartz)return CLOCK_HSE; else return CLOCK_HSI;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//  SysTick ///////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define SysTick_CTLR_STE (1<<0) //systick enable
#define SysTick_CTLR_STIE (1<<1) //interrupt enable
#define SysTick_CTLR_STCLK (1<<2) //clock source: 0-HCLK/8, 1-HCLK/1
#define SysTick_CTLR_STRE (1<<3) //auto reload enable
#define SysTick_CTLR_MODE (1<<4) //up/down. 0-up, 1-down
#define SysTick_CTLR_INIT (1<<31) //1: Updated to 0 when upcounting, while updated to the comparison value when downcounting.

#define SysTick_SR_CNTIF (1<<0) //interrupt flag


static inline void systick_init(){
  SysTick->SR=0;
  SysTick->CNT=0;
  
  SysTick->CTLR = SysTick_CTLR_STCLK | SysTick_CTLR_STE;
}

static inline int32_t systick_read32(){
  //return SysTick->CNT;
  return ((volatile int32_t*)&(SysTick->CNT))[0]; //Спасибо наркоманам из WCH, которые не дают доступа к CNTH:CNTL
}

static inline uint64_t systick_read64(){
  return SysTick->CNT;
}

static void delay_ticks(int32_t t){
  int32_t t_prev = systick_read32();
  while( (systick_read32() - t_prev) < t ){}
}

#endif
