/*
В CH32 проблема с большими буферами: DMA1 не умеет пересекать границу 64к.
В данной версии библиотеки рекомендуется назначить хотя бы один SPI на SPI3
поскольку он висит на DMA2, которое границу пересекать умеет. Какой именно
канал будет на SPI3 - безразлично, библиотека сама подберет.
*/

#if 1==0
//1: clock from MASTER-SPI
  #define SPIOSCI		3,-1,2 // SPI3=master, SPI1(remap)=slave, SPI2=slave
  #define SPIOSCI_SPEED 2 // F_APB( SPI3 ) / 2
  #define SPIOSCI_SZ	1000
  #include "spi_osci.h"
//2: clock from Timer
  #define SPIOSCI			3,1,-2 // all SPI = slaves (SPI2 - remap)
  #define SPIOSCI_TIMER		10,1,TIMO_PWM_NINV | TIMO_NEG | TIMO_REMAP1 //TIM10.1N, remap1
  #define SPIOSCI_TIMOUT	A,5,1
  #define SPIOSCI_SPEED 	2 // F_APB(Tim10) / 2
  #define SPIOSCI_SZ		1000
  #define SPIOSCI_LOGSIZE	1000 // (may be undefined) text buffer to comments as ch4 UART text string
  #include "spi_osci.h"
//Common:
  void spiosci_uart(uint16_t idx, uint16_t cnt, uint8_t chan1, uint8_t chan2, uint8_t chan3, uint8_t log){
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
...
  spiosci_init();
  spiosci_start();
  //Do smth
  spiosci_stop();
  spiosci_log("Some info");
  spiosci_out(spiosci_uart);
#endif


#ifdef __SPIOSCI_PINS__

#if __SPIOSCI_PINS__ == 1	//SPI_1
  #define SPIn			1
  #define SPI_MISO		A,6,1
  #define SPI_MOSI		A,7,1
  #define SPI_SCK		A,5,1
#elif __SPIOSCI_PINS__== -1	//SPI_1, remap
  #define SPIn			1
  #define SPI_REMAP
  #define SPI_MISO		B,4,1
  #define SPI_MOSI		B,5,1
  #define SPI_SCK		B,3,1
#elif __SPIOSCI_PINS__ == 2	//SPI_2
  #define SPIn			2
  #define SPI_MISO		B,14,1
  #define SPI_MOSI		B,15,1
  #define SPI_SCK		B,13,1
#elif __SPIOSCI_PINS__ == 3	//SPI_3
  #define SPIn			3
  #define SPI_MISO		B,4,1
  #define SPI_MOSI		B,5,1
  #define SPI_SCK		B,3,1
#elif __SPIOSCI_PINS__== -3	//SPI_3, remap
  #define SPIn			3
  #define SPI_REMAP
  #define SPI_MISO		C,11,1
  #define SPI_MOSI		C,12,1
  #define SPI_SCK		C,10,1
#else
  #error SPIOSCI does not support software SPI
#endif
#undef __SPIOSCI_PINS__
#else



#ifndef __SPI_OSCI_H__
#define __SPI_OSCI_H__

#include "dma.h"

#ifdef SPIOSCI_TIMER
  #ifndef SPIOSCI_TIMOUT
    #error SPIOSCI_TIMOUT (timer output GPIO) undefined
  #endif
  #ifndef SPIOSCI_SPEED
    #warning SPIOSCI_SPEED undefined. Use default value (1:256)
    #define SPIOSCI_SPEED 256
  #endif
  #define SPIOSCI_SPEED_DIV 2
  #include "timer.h"
#else //SPI(ch1) = master, others = slaves
  #ifdef SPIOSCI_SPEED
    #define SPIOSCI_SPEED_DIV SPIOSCI_SPEED
  #else
    #warning SPIOSCI_SPEED undefined. Use default value (1:256)
    #define SPIOSCI_SPEED_DIV 256
  #endif
#endif

#define nums SPIOSCI,0,0,0
#define ch1	marg1(nums)
#define ch2 marg2(nums)
#define ch3 marg3(nums)

#define __SPIOSCI_PINS__ ch1
#include "spi_osci.h"
#define SPI_SPEED_DIV	SPIOSCI_SPEED_DIV
#define SPI_LSBFIRST	1
#define SPI_PHASE		0
#undef SPI_MISO
#define SPI_MODE		SPI_SLAVE
#include "spi.h"
#undef SPIn
#if ch1 == -1
  #undef ch1
  #define ch1 1
#elif ch1 == -2
  #undef ch1
  #define ch1 2
#elif ch1 == -3
  #undef ch1
  #define ch1 3
#endif

void dma_register(SPI_DMA_RX(ch1)){}
#ifndef SPIOSCI_TIMER
  void dma_register(SPI_DMA_TX(ch1)){}
#endif

#if ch2 != 0
  #define __SPIOSCI_PINS__ ch2
  #include "spi_osci.h"
  #undef SPI_MISO
  #define SPI_SPEED_DIV	SPIOSCI_SPEED_DIV
  #define SPI_LSBFIRST	1
  #define SPI_PHASE		0
  #define SPI_MODE		SPI_SLAVE
  #include "spi.h"
  #undef SPIn
  #if ch2 == -1
    #undef ch2
    #define ch2 1
  #elif ch2 == -2
    #undef ch2
    #define ch2 2
  #elif ch2 == -3
    #undef ch2
    #define ch2 3
  #endif
  void dma_register(SPI_DMA_RX(ch2)){}
  void dma_register(SPI_DMA_TX(ch1)){}
  
  #if ch3 != 0
    #define __SPIOSCI_PINS__ ch3
    #include "spi_osci.h"
    #undef SPI_MISO
    #define SPI_SPEED_DIV	SPIOSCI_SPEED_DIV
    #define SPI_LSBFIRST	1
    #define SPI_PHASE		0
    #define SPI_MODE		SPI_SLAVE
    #include "spi.h"
    #undef SPIn
    #if ch3 == -1
      #undef ch3
      #define ch3 1
    #elif ch3 == -2
      #undef ch3
      #define ch3 2
    #elif ch3 == -3
      #undef ch3
      #define ch3 3
    #endif
    #define chbuf3(i) spiosci_buf[2][i]
    void dma_register(SPI_DMA_RX(ch3)){}
    #define SPIOSCI_channels	3
  #else  //marg3 == 0
    #define SPIOSCI_channels	2
  #endif //marg3 ?= 0
#else  //marg2 == 0
  #define SPIOSCI_channels	1
#endif //marg2 ?= 0

__attribute__((aligned(2)))volatile uint8_t spiosci_buf[SPIOSCI_channels][SPIOSCI_SZ];
uint8_t spiosci_idx[4] = {0, 1, 2, 0}; //idx[3] != 0 - error

#define buf_broken(buf) ( ( ((uint32_t)buf) ^ ((uint32_t)buf+SPIOSCI_SZ) ) >> 16 )
void spiosci_test64k_bound(){
  if( buf_broken(spiosci_buf[0]) ){
  #if ch1 == 3
    spiosci_idx[0] = 0; spiosci_idx[1] = 1; spiosci_idx[2] = 2;
  #elif ch2 == 3
    spiosci_idx[0] = 1; spiosci_idx[1] = 0; spiosci_idx[2] = 2;
  #elif ch3 == 3
    spiosci_idx[0] = 2; spiosci_idx[1] = 1; spiosci_idx[2] = 0;
  #else
    spiosci_idx[3] = 1;
  #endif
  }else if( (SPIOSCI_channels>1) && buf_broken(spiosci_buf[1]) ){
  #if ch1 == 3
    spiosci_idx[0] = 1; spiosci_idx[1] = 0; spiosci_idx[2] = 2;
  #elif ch2 == 3
    spiosci_idx[0] = 0; spiosci_idx[1] = 1; spiosci_idx[2] = 2;
  #elif ch3 == 3
    spiosci_idx[0] = 0; spiosci_idx[1] = 2; spiosci_idx[2] = 1;
  #else
    spiosci_idx[3] = 1;
  #endif
  }else if( (SPIOSCI_channels>2) && buf_broken( spiosci_buf[2]) ){
  #if ch1 == 3
    spiosci_idx[0] = 2; spiosci_idx[1] = 1; spiosci_idx[2] = 0;
  #elif ch2 == 3
    spiosci_idx[0] = 0; spiosci_idx[1] = 2; spiosci_idx[2] = 1;
  #elif ch3 == 3
    spiosci_idx[0] = 0; spiosci_idx[1] = 1; spiosci_idx[2] = 2;
  #else
    spiosci_idx[3] = 1;
  #endif
  }
}
#undef buf_broken


void spiosci_init(){
  spiosci_test64k_bound();
  
  SPI_init(ch1);
  SPI_size_16(ch1);
  dma_clock(SPI_DMA_RX(ch1), 1);
  dma_disable(SPI_DMA_RX(ch1));
  
  dma_cfg_io(SPI_DMA_RX(ch1), spiosci_buf[spiosci_idx[0]], &SPI_DATA(ch1), SPIOSCI_SZ/2);
  dma_cfg_mem(SPI_DMA_RX(ch1), 16,1, 16,0, 0, DMA_PRI_VHIGH);
  dma_enable(SPI_DMA_RX(ch1));
  
#if ch2 != 0
  SPI_init(ch2);
  SPI_size_16(ch2);
  dma_clock(SPI_DMA_RX(ch2), 1);
  dma_disable(SPI_DMA_RX(ch2));
  
  dma_cfg_io(SPI_DMA_RX(ch2), spiosci_buf[spiosci_idx[1]], &SPI_DATA(ch2), SPIOSCI_SZ/2);
  dma_cfg_mem(SPI_DMA_RX(ch2), 16,1, 16,0, 0, DMA_PRI_VHIGH);
  dma_enable(SPI_DMA_RX(ch2));
#endif
  
#if ch3 != 0
  SPI_init(ch3);
  SPI_size_16(ch3);
  dma_clock(SPI_DMA_RX(ch3), 1);
  dma_disable(SPI_DMA_RX(ch3));
  
  dma_cfg_io(SPI_DMA_RX(ch3), spiosci_buf[spiosci_idx[2]], &SPI_DATA(ch3), SPIOSCI_SZ/2);
  dma_cfg_mem(SPI_DMA_RX(ch3), 16,1, 16,0, 0, DMA_PRI_VHIGH);
  dma_enable(SPI_DMA_RX(ch3));
#endif
  
#ifndef SPIOSCI_TIMER
  dma_clock(SPI_DMA_TX(ch1), 1);
  dma_disable(SPI_DMA_TX(ch1));
  dma_cfg_io(SPI_DMA_TX(ch1), &SPI_DATA(ch1), spiosci_buf[0], SPIOSCI_SZ/2);
  dma_cfg_mem(SPI_DMA_TX(ch1), 16,0, 16,0, 0, DMA_PRI_VHIGH);
#else
  GPIO_manual(SPIOSCI_TIMOUT, GPIO_APP50);
  timer_init(SPIOSCI_TIMER, SPIOSCI_SPEED/2-1, 2-1);
  timer_chval(SPIOSCI_TIMER) = 1;
  timer_chcfg(SPIOSCI_TIMER);
  TIMO_OFF(SPIOSCI_TIMER);
  timer_enable(SPIOSCI_TIMER);
#endif
}

#ifndef SPIOSCI_TIMER
  void spiosci_start(){
    dma_enable(SPI_DMA_TX(ch1));
  }
  void spiosci_stop(){
    dma_disable(SPI_DMA_TX(ch1));
  }
#else
  void spiosci_start(){
    TIMO_DEF(SPIOSCI_TIMER);
  }
  void spiosci_stop(){
    TIMO_OFF(SPIOSCI_TIMER);
  }
#endif

uint16_t spiosci_count(){
  uint32_t sz = SPIOSCI_SZ - 2*(DMA_CH(SPI_DMA_RX(ch1))->CNTR);
/*#if ch2 != 0
  uint32_t sz2 = SPIOSCI_SZ - DMA_CH(SPI_DMA_RX(ch2))->CNTR;
  if(sz2 < sz)sz = sz2;
  #if ch3 != 0
    uint32_t sz3 = SPIOSCI_SZ - DMA_CH(SPI_DMA_RX(ch3))->CNTR;
    if(sz3 < sz)sz = sz3;
  #endif
#endif*/
  return sz;
//  return SPIOSCI_SZ - DMA_CH(SPI_DMA_RX(ch1))->CNTR;
}

#ifdef SPIOSCI_LOGSIZE
uint8_t spiosci_logbuf[SPIOSCI_LOGSIZE];
volatile uint16_t spiosci_lcount = 0;
void spiosci_log(char *str){
  while(str[0]){
    spiosci_logbuf[ spiosci_lcount ] = str[0];
    spiosci_lcount++;
    str++;
  }
}

void spiosci_resetlog(){
  spiosci_lcount = 0;
}

uint8_t spiosci_outbuf(uint32_t i){
  if( i < 10 )return 0xFF;
  i -= 10;
  uint32_t idx = (i * 8 / 10);
  uint8_t offs = (i * 8 % 10);
  if(idx >= spiosci_lcount)return 0xFF;
  uint8_t a = spiosci_logbuf[idx];
  uint16_t res;
  if( idx < spiosci_lcount-1 ){
    uint8_t b = spiosci_logbuf[idx + 1];
    res = 0 | (a<<1) | (1<<9) | (0<<10) | (b<<11);
  }else{
    res = 0 | (a<<1) | (1<<9) | (1<<10) | (0xFFFF<<11);
  }
  res >>= offs;
  return res;
}
#else
void spiosci_log(char *str){}
uint8_t spiosci_outbuf(uint32_t i){return 0;}
#endif

typedef void (*spiosci_outfunc_t)(uint16_t idx, uint16_t max, uint8_t chan1, uint8_t chan2, uint8_t chan3, uint8_t log);

#define chbuf1(i)	spiosci_buf[spiosci_idx[0]][i]
#if ch2 != 0
  #define chbuf2(i)	spiosci_buf[spiosci_idx[1]][i]
#else
  #define chbuf2(i)	0
#endif
#if ch3 != 0
  #define chbuf3(i)	spiosci_buf[spiosci_idx[2]][i]
#else
  #define chbuf3(i)	0
#endif

void spiosci_out(spiosci_outfunc_t outfunc){
  uint32_t cnt = spiosci_count();
  if(spiosci_idx[3] == 0){
    for(uint32_t i=0; i<cnt; i++){
      outfunc( i, cnt, chbuf1(i), chbuf2(i), chbuf3(i), spiosci_outbuf(i) );
      //outfunc( spiosci_buf[0][i], 0, 0 );
    }
  }else{
    spiosci_resetlog();
    spiosci_log("Buffer crosses the 64k boundary; One channel must be SPI3");
    for(uint32_t i=0; i<100; i++){
      outfunc( i, 100, 0, 0, 0, spiosci_outbuf(i) );
    }
  }
}

#undef ch1
#undef ch2
#undef ch3
#undef chbuf1
#undef chbuf2
#undef chbuf3
#undef nums

#endif //__SPI_OSCI_H__

#endif //__SPIOSCI_PINS__