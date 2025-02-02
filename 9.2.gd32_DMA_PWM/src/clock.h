#ifndef __CLOCK_H__
#define __CLOCK_H__

#if HXTAL_VALUE != 8000000
  #error This library is for 8 MHz quartz only
#endif
//HSE=HXTAL_VALUE=8000000
//HSI = 8 MHz
#define CLOCK_FAIL -1
#define CLOCK_HSE 2
#define CLOCK_HSI 1
int8_t clock_HS(char quartz){
  int i;
  uint32_t tmp;
  RCU_CTL &=~ RCU_CTL_HXTALEN;
  RCU_CTL &=~ RCU_CTL_HXTALBPS;
  RCU_CTL &=~ RCU_CTL_PLLEN;
  
  if(quartz){
    RCU_CTL |= RCU_CTL_HXTALEN;
    for(i=0;i<0x0FFF;i++){
      if(RCU_CTL & RCU_CTL_HXTALSTB){i=0x1FFF; break;}
    }
    if(i != 0x1FFF)quartz = 0;
  }
  
  if(!quartz){
    RCU_CTL |= RCU_CTL_IRC8MEN;
    for(i=0;i<0x0FFF;i++){
      if(RCU_CTL & RCU_CTL_IRC8MSTB){i=0x1FFF; break;}
    }
    if(i != 0x1FFF)return CLOCK_FAIL;
  }
  
  tmp = RCU_CFG0;
  tmp &=~(RCU_CFG0_PLLMF | RCU_CFG0_PLLSEL);
  if(quartz){
    tmp |= RCU_CFG0_PLLSEL;
    RCU_CFG1 = (RCU_CFG1 & RCU_CFG1_PREDV0) | RCU_PREDV0_DIV2; //8/2 = 4 MHz
  }
  
  tmp |= RCU_PLL_MUL27; //CK_SYS = 4*27 = 108 MHz
  
  tmp &=~ (RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC);
  tmp |= RCU_AHB_CKSYS_DIV1; //AHB = CK_SYS/1 = 108 MHz
  tmp |= RCU_APB1_CKAHB_DIV2; //APB1 = AHB/2 = 54 MHz
  tmp |= RCU_APB2_CKAHB_DIV2; //APB2 = AHB/2 = 54 MHz
  
  RCU_CFG0 = tmp;
  
  RCU_CTL |= RCU_CTL_PLLEN;
  for(i=0;i<0x0FFF;i++){
    if(RCU_CTL & RCU_CTL_PLLSTB){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)return CLOCK_FAIL;
  
  tmp = RCU_CFG0;
  tmp &=~RCU_CFG0_SCS;
  tmp |= RCU_CKSYSSRC_PLL;
  RCU_CFG0 = tmp;
  for(i=0;i<0x0FFF;i++){
    if((RCU_CFG0 & RCU_CFG0_SCSS)==RCU_SCSS_PLL){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)return CLOCK_FAIL;
  
  SystemCoreClock = 108000000;
  
  if(quartz)return CLOCK_HSE; else return CLOCK_HSI;
}


inline uint32_t read_mcycles(){
  uint32_t res;
  asm volatile("csrr %0, mcycle" : "=r"(res) );
  return res;
}

void delay_ticks(uint32_t cycles){
  int32_t cur_cycles, av_cycles;
  av_cycles = read_mcycles() + cycles;
  do{cur_cycles = read_mcycles();}while( (av_cycles - cur_cycles) > 0 );
}

void delay_us(uint32_t us){
  int32_t cur_cycles, av_cycles;
  av_cycles = read_mcycles() + (us * (SystemCoreClock / 1000000));
  do{cur_cycles = read_mcycles();}while( (av_cycles - cur_cycles) > 0 );
}

void delay_ms(uint32_t ms){
  int32_t cur_cycles, av_cycles;
  av_cycles = read_mcycles() + (ms * (SystemCoreClock / 1000 ));
  do{cur_cycles = read_mcycles();}while( (av_cycles - cur_cycles) > 0 );
}

#endif