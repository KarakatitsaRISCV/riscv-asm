#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"

uint32_t SystemCoreClock = 8000000;
#define F_SYS	8000000

inline uint32_t read_mcycle(){
  uint32_t res;
  asm volatile("csrr %0, mcycle" : "=r"(res) );
  return res;
}
void delay_cycles(uint32_t cycles){
  int32_t cyc_av = read_mcycle() + cycles;
  int32_t cyc_cur;
  do{
    cyc_cur = read_mcycle();
  }while( (cyc_av - cyc_cur) > 0);
}
void delay_us(uint32_t us){
  int32_t cyc_av = read_mcycle() + us * (8000000 / 1000000);
  int32_t cyc_cur;
  do{
    cyc_cur = read_mcycle();
  }while( (cyc_av - cyc_cur) > 0);
}

const char uart_data[] = "Возможно, если бы у меня изначально был человеческий JTAG-программатор, было бы проще. Но моя нелюбовь покупать готовые отладочные устройства этому активно противится. Как бы то ни было, проблема решена, причем с избытком. Описываю я только свое решение, возможно с другими программаторами, контроллерами или софтом алгоритм будет отличаться.\r\n";

void task1(){ //LEDs
  static uint8_t state = 0;
  static int32_t t_av = 0;
  const int32_t t_delay_cyc = 200 * (F_SYS / 1000);
  int32_t t_cur = read_mcycle();
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
  if( ! (USART_STAT(USART0) & USART_STAT_TBE) )return;
  USART_DATA(USART0) = uart_data[idx];
  idx++;
  if(idx > sizeof(uart_data))idx = 0;
}

int main(){
  eclic_init( ECLIC_NUM_INTERRUPTS );
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  
  USART_BAUD(USART0) = 8000000 / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_UEN;
  
  while(1){
    task1();
    task2();
  }
}