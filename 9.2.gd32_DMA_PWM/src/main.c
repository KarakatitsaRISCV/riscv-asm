#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"
#include "clock.h"

uint32_t SystemCoreClock = 8000000;

uint32_t pwm_buf[256];
#define AVAIBLE_LEDS	(1<<5 | 1<<6 | 1<<7)

void pwm_init(){
  for(int i=0; i<256; i++)pwm_buf[i] = (AVAIBLE_LEDS<<16);
  
  RCU_APB1EN |= RCU_APB1EN_TIMER3EN;
  TIMER_PSC(TIMER3) = (1 - 1);
  TIMER_CAR(TIMER3) = (108 - 1);
  TIMER_CH0CV(TIMER3) = 1;
  
  RCU_AHBEN |= RCU_AHBEN_DMA0EN;
  DMA_CHCTL(DMA0, 0) = 0;
  DMA_CHPADDR(DMA0, 0) = (uint32_t)&GPIO_BOP(GPIOB);
  DMA_CHMADDR(DMA0, 0) = (uint32_t)pwm_buf;
  DMA_CHCNT(DMA0, 0) = sizeof(pwm_buf)/sizeof(pwm_buf[0]);
  DMA_CHCTL(DMA0, 0) = DMA_PRIORITY_ULTRA_HIGH
    | DMA_PERIPHERAL_WIDTH_32BIT // per data size: 32
    | DMA_MEMORY_WIDTH_32BIT     // mem data size: 32
    | (0*DMA_CHXCTL_PNAGA) // Autoincrement per: disable
    | (1*DMA_CHXCTL_MNAGA) // Autoincrement mem: enable
    | (1*DMA_CHXCTL_CMEN)  // Circular mode: enable
    | (1*DMA_CHXCTL_DIR);  // Direction: mem -> per
  DMA_CHCTL(DMA0, 0) |= DMA_CHXCTL_CHEN;
  
  TIMER_DMAINTEN(TIMER3) |= TIMER_DMAINTEN_CH0DEN;
  TIMER_CTL0(TIMER3) |= TIMER_CTL0_CEN;
}

void pwm_set(uint8_t chan, uint8_t val){
  uint32_t mask = (1<<chan);
  for(int i=0; i<val; i++)pwm_buf[i] |= mask;
  mask =~mask;
  for(int i=val; i<256; i++)pwm_buf[i] &= mask;
}

int main(){
  clock_HS(1);
  
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  
  pwm_init();
  
  uint8_t pwm_val[3] = {10, 50, 200};

  while(1){
    pwm_val[0]++; pwm_set(5, pwm_val[0]);
    pwm_val[1]++; pwm_set(6, pwm_val[1]);
    pwm_val[2]++; pwm_set(7, pwm_val[2]);
    
    delay_ticks(108000000/10);
  }
}


