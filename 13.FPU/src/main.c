#include "ch32v30x.h"
#include "hardware.h"
#include "pinmacro.h"
#define USART 1
#define UART_SIZE_PWR 6
#include "uart.h"

#ifndef NULL
  #define NULL ((void*)0)
#endif
void SystemInit(void){}
void sleep(uint32_t t){while(t--)asm volatile("nop");}

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
  return ((volatile int32_t*)&(SysTick->CNT))[0];
}

//Вывод числа с фиксированной точкой, [frac] знаков после точки
//например, uart_fpi32(123, 0) выведет "123"
//          uart_fpi32(123, 2) выведет "1.23"
//          uart_fpi32(123, 5) выведет "0.00123"
void uart_fpi32(int32_t val, uint32_t frac){
  char buf[13];
  char sign = 0;
  if(val < 0){sign = 1; val = -val;}
  char *ch = &buf[12];
  ch[0] = 0;
  int dig;
  for(dig=0; dig<frac; dig++){
    ch--;
    ch[0] = (val % 10) + '0';
    val /= 10;
  }
  if(frac > 0){ch--; ch[0] = '.';}
  do{
    ch--;
    ch[0] = (val % 10) + '0';
    val /= 10;
  }while(val);
  if(sign){ch--; ch[0] = '-';}
  UART_puts(USART, ch);
}

int main(){
  systick_init();
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPO_OFF(RLED); GPO_OFF(GLED); GPO_OFF(YLED);
  
  UART_init(USART, 8000000 / 9600);
  
  UART_puts(USART, "\r\n\r\n");
  UART_puts(USART, __TIME__ " " __DATE__ "\r\n");
  
  //Вычисления на float-ах
  uint32_t t_prev = systick_read32();
  
  volatile float x = 1.1;
  volatile float res = 0;
  for(int i=0; i<9; i++)res += x;
  
  uint32_t t_cur = systick_read32();
  
  UART_puts(USART, "Float:");
  uart_fpi32(res*100000000, 8);
  UART_puts(USART, "\r\nt=");
  uart_fpi32( t_cur - t_prev, 0 );
  UART_puts(USART, "\r\n");
  
  
  sleep(100000);
  
  //вычисления на fixed-point-ах
  t_prev = systick_read32();
  
  volatile uint32_t y = 110000000; //1.1 * 10⁸
  volatile uint32_t ires = 0;
  for(int i=0; i<9; i++)ires += y;
  
  t_cur = systick_read32();
  
  UART_puts(USART, "Fixed-point:");
  uart_fpi32(ires, 8);
  UART_puts(USART, "\r\nt=");
  uart_fpi32( t_cur - t_prev, 0 );
  UART_puts(USART, "\r\n");
  
  while(1){
    GPO_T(YLED);
    sleep(400000);
  }
}