#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"

uint32_t SystemCoreClock = 8000000;
#define F_SYS	8000000

inline int32_t read_mcycle(){
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

void os_start();
void task_create( void(*func)(void) );
void yield();

void task1(){ //LEDs
  static int32_t t_av = 0;
  const int32_t t_delay_cyc = 200 * (F_SYS / 1000);
  GPO_OFF(GLED); GPO_OFF(RLED);
  while(1){
    GPO_ON(GLED);
    
    t_av = read_mcycle() + t_delay_cyc;
    while( (read_mcycle() - t_av) < 0 )yield();
    
    GPO_OFF(GLED);
    
    t_av = read_mcycle() + t_delay_cyc;
    while( (read_mcycle() - t_av) < 0 )yield();
    
    GPO_ON(RLED);
    
    t_av = read_mcycle() + t_delay_cyc;
    while( (read_mcycle() - t_av) < 0 )yield();
    
    GPO_OFF(RLED);
    
    t_av = read_mcycle() + t_delay_cyc;
    while( (read_mcycle() - t_av) < 0 )yield();
  }
}

void task2(){ //UART
  uint16_t idx = 0;
  while(1){
    while( ! (USART_STAT(USART0) & USART_STAT_TBE) )yield();
    USART_DATA(USART0) = uart_data[idx];
    yield();
    idx++;
    if(idx > sizeof(uart_data))idx = 0;
  }
}


int main(){
  eclic_init( ECLIC_NUM_INTERRUPTS );
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  
  USART_BAUD(USART0) = 8000000 / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_UEN;
  
  task_create(task1);
  task_create(task2);
  
  os_start();
  
  while(1){
  }
}