#include "ch32v30x.h"
#include "hardware.h"
#include "pinmacro.h"

void SystemInit(void){}
void sleep(uint32_t t){while(t--)asm volatile("nop");}

int main(){
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED);
  GPO_OFF(RLED); GPO_OFF(GLED);
  
  while(1){
    GPO_OFF(GLED); GPO_ON(RLED);
    sleep(500000);
    GPO_OFF(RLED); GPO_ON(GLED);
    sleep(500000);
  }
}
