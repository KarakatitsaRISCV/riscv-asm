#include "ch32v30x.h"
#include "clock.h"
#include "pinmacro.h"
#define USART 1
#define UART_SIZE_PWR 8
#include "uart.h"

#ifndef NULL
  #define NULL ((void*)0)
#endif
void SystemInit(void){}

#define F_SYS	8000000
#define UART_TX A,9 ,1,GPIO_APP50
#define UART_RX A,10,1,GPIO_HIZ
#if 1
#define RLED	B,4 ,0,GPIO_PP50
#define GLED	A,11,1,GPIO_PP50
#else
#define RLED	B,5,1,GPIO_PP50
#define GLED	B,7,1,GPIO_PP50
#endif

const char uart_data[] = "Возможно, если бы у меня изначально был человеческий JTAG-программатор, было бы проще. Но моя нелюбовь покупать готовые отладочные устройства этому активно противится. Как бы то ни было, проблема решена, причем с избытком. Описываю я только свое решение, возможно с другими программаторами, контроллерами или софтом алгоритм будет отличаться.\r\n";

void os_start();
void task_create( void(*func)(void) );

void task1(){ //LEDs
  GPO_OFF(GLED); GPO_OFF(RLED);
  const int32_t t_delay_cyc = 200 * (F_SYS / 1000);
  while(1){
    GPO_ON(GLED);
    delay_ticks(t_delay_cyc);
    GPO_OFF(GLED);
    delay_ticks(t_delay_cyc);
  }
}

void task2(){ //UART
  uint16_t idx = 0;
  while(1){
    while( !(USART1->STATR & USART_STATR_TXE) ){};
    USART1->DATAR = uart_data[idx];
    idx++;
    if(idx > sizeof(uart_data))idx = 0;
  }
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
  
  task_create(task1);
  task_create(task2);
  
  os_start();
  
  
  while(1){
  }
}