#include "ch32v30x.h"
#include "clock.h"
#include "pinmacro.h"

#ifndef NULL
  #define NULL ((void*)0)
#endif
void SystemInit(void){}

#define F_SYS	8000000
#define UART_TX A,9 ,1,GPIO_APP50
#define UART_RX A,10,1,GPIO_HIZ
#define RLED	B,4 ,0,GPIO_PP50
#define GLED	A,11,1,GPIO_PP50

const char uart_data[] = "Возможно, если бы у меня изначально был человеческий JTAG-программатор, было бы проще. Но моя нелюбовь покупать готовые отладочные устройства этому активно противится. Как бы то ни было, проблема решена, причем с избытком. Описываю я только свое решение, возможно с другими программаторами, контроллерами или софтом алгоритм будет отличаться.\r\n";

void task1(){ //LEDs
  static uint8_t state = 0;
  static int32_t t_av = 0;
  const int32_t t_delay_cyc = 200 * (F_SYS / 1000);
  int32_t t_cur = systick_read32();
  switch(state){
    case 0:
      if( (t_cur - t_av) > 0){
        t_av = t_cur + t_delay_cyc;
        GPO_OFF(GLED);
        state = 1;
      }
      break;
    case 1:
      if( (t_cur - t_av) > 0){
        t_av = t_cur + t_delay_cyc;
        GPO_ON(RLED);
        state = 2;
      }
      break;
    case 2:
      if( (t_cur - t_av) > 0){
        t_av = t_cur + t_delay_cyc;
        GPO_OFF(RLED);
        state = 3;
      }
      break;
    case 3:
      if( (t_cur - t_av) > 0){
        t_av = t_cur + t_delay_cyc;
        GPO_ON(GLED);
        state = 0;
      }
      break;
    default: state = 0;
  }
}

void task2(){ //UART
  static uint16_t idx = 0;
  if( !(USART1->STATR & USART_STATR_TXE) )return;
  USART1->DATAR = uart_data[idx];
  idx++;
  if(idx > sizeof(uart_data))idx = 0;
}

int main(){
  systick_init();

  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED);
  GPO_OFF(RLED); GPO_OFF(GLED);
  
  GPIO_config( UART_TX );
  GPIO_config( UART_RX );
  RCC->APB2PCENR |= RCC_USART1EN;
  USART1->BRR = 8000000 / 9600;
  USART1->CTLR1 = USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE;
  
  while(1){
    task1();
    task2();
  }
}