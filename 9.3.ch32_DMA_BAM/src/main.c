#include "ch32v30x.h"
#include "hardware.h"
#include "clock.h"
#include "timer.h"
#include "dma.h"
#define USART 1
#define UART_SIZE_PWR 10
#include "uart.h"

#ifndef NULL
  #define NULL ((void*)0)
#endif
void SystemInit(void){}

#define TIM_ch1	4,1,TIMO_NONE
#define TIM_ch2	4,2,TIMO_NONE
void dma_register(timer_dma(TIM_ch1)){}
void dma_register(timer_dma(TIM_ch2)){}


uint32_t leds_data[8];
uint16_t tim_top[8] = {256, 128, 64, 32, 16, 8, 4, 512};
#define AVAIBLE_LEDS	(1<<5 | 1<<6 | 1<<7)

void pwm_init(){
  for(int i=0; i<8; i++)leds_data[i] = (AVAIBLE_LEDS<<16);
  timer_init(TIM_ch1, 10-1, 100);
  timer_chval(TIM_ch1) = 1;
  TIM4->CTLR1 = TIM_ARPE;
  
  dma_clock(timer_dma(TIM_ch1), 1);
  dma_cfg_io(timer_dma(TIM_ch1), &(GPIOB->BSHR), leds_data, 8);
  dma_cfg_mem(timer_dma(TIM_ch1), 32,0, 32,1, 1, DMA_PRI_VHIGH);
  dma_enable(timer_dma(TIM_ch1));
  dma_enable(timer_dma(TIM_ch1));
  
  timer_chval(TIM_ch2) = 1;
  dma_clock(timer_dma(TIM_ch2), 1);
  dma_cfg_io(timer_dma(TIM_ch2), &(TIMx(TIM_ch2)->ATRLR), tim_top, 8);
  dma_cfg_mem(timer_dma(TIM_ch2), 16,0, 16,1, 1, DMA_PRI_VHIGH);
  dma_enable(timer_dma(TIM_ch2));
  
  timer_enable(TIM_ch1);
}

void pwm_set(uint8_t chan, uint8_t val){
  uint32_t mask = (1<<chan);
  uint32_t nmask = ~mask;
  for(int i=0; i<8; i++){
    if(val & 0x80)leds_data[i] |= mask; else leds_data[i] &= nmask;
    val <<= 1;
  }
}

#define SPIOSCI			-1 // SPI1, remap, slave; MOSI = PB5
#define SPIOSCI_TIMER	2,2,TIMO_PWM_NINV | TIMO_POS | TIMO_REMAP1
#define SPIOSCI_TIMOUT	B,3,1
#define SPIOSCI_SPEED 	2
#define SPIOSCI_SZ		20000
#define SPIOSCI_LOGSIZE	1000
#include "spi_osci.h"
void spiosci_uart(uint16_t idx, uint16_t max, uint8_t chan1, uint8_t chan2, uint8_t chan3, uint8_t log){
  for(int i=0; i<8; i++){
    while(UART_avaible(USART) < 16){}
    uint8_t a = chan1 & 1;
    uint8_t b = chan2 & 1;
    uint8_t c = chan3 & 1;
    uint8_t l = log & 1;
    UART_putc(USART, 0x80 | a | (b<<1) | (c<<2) | (l<<4));
    chan1 >>= 1;
    chan2 >>= 1;
    chan3 >>= 1;
    log >>= 1;
  }
}

int main(){
  clock_HS(1);
  systick_init();

  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPO_OFF(RLED); GPO_OFF(GLED); GPO_OFF(YLED);
  
  UART_init(USART, 144000000 / 2 / 115200);
  
  pwm_init();
  pwm_set(5, 0x55);
  
  spiosci_init();
  GPIO_config(RLED);
  spiosci_start();
  delay_ticks(144000000/10);
  spiosci_stop();
  spiosci_log(__TIME__);
  spiosci_out(spiosci_uart);
  
  uint8_t pwm_val[3] = {0, 85, 170};
  while(1){
    pwm_val[0]++; pwm_set(5, pwm_val[0]);
    pwm_val[1]++; pwm_set(6, pwm_val[1]);
    pwm_val[2]++; pwm_set(7, pwm_val[2]);
    delay_ticks(144000000/100);
  }
}