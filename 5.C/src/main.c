#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"

uint32_t SystemCoreClock = 8000000;

int main(){
  eclic_init( ECLIC_NUM_INTERRUPTS );
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  GPO_ON(GLED);
  
  USART_BAUD(USART0) = 8000000 / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_TBEIE;
  
  eclic_set_vmode( USART0_IRQn );
  eclic_enable_interrupt( USART0_IRQn );
  
  eclic_global_interrupt_enable();
  
  while(1){
    GPO_T(YLED);
    if(GPI_ON(SBTN))GPO_ON(RLED); else GPO_OFF(RLED);
    for(uint32_t i=0; i<100000; i++)asm volatile("nop");
  }
}

void USART0_IRQHandler(){
  USART_CTL0(USART0) = USART_CTL0_REN | USART_CTL0_TEN | USART_CTL0_UEN;
  USART_DATA(USART0) = 'U';
}
