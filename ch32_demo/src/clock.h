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
      #ifdef CH32V30x_D8
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL18);
      #else
        RCC->CFGR2 = 0;
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL18_EXTEN);
      #endif
    }else{
      quartz = 0;
    }
  }
  if(!quartz){
    EXTEN->EXTEN_CTR |= EXTEN_PLL_HSI_PRE;
    RCC->CFGR0 |= RCC_PLLSRC_HSI_Div2 | RCC_PLLMULL18;
  }
  
  RCC->CTLR |= RCC_PLLON;
  while(!(RCC->CTLR & RCC_PLLRDY)){}
  
  RCC->CFGR0 = (RCC->CFGR0 &~ RCC_SW) | RCC_SW_PLL;
  while((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL){}
  
  if(quartz)return CLOCK_HSE; else return CLOCK_HSI;
}

#endif
