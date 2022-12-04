#include <stm32l151xc.h>
#include "hardware.h"
#include "pinmacro.h"
#include "usb_lib.h"
#include "clock.h"
#include "systick.h"

void __attribute__((weak)) _init(void){}
void __attribute__((weak)) SystemInit(void){}

void sleep(uint32_t time){
  while(time--)asm volatile("nop");
}

int main(void){
  clock_HS(1);
  systick_init();
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;

  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(BLED);
  GPIO_config(ADC_IN); GPIO_config(RESET); GPIO_config(BOOT0);
  
  GPO_OFF(RLED); GPO_OFF(GLED); GPO_OFF(BLED);
  GPO_OFF(RESET); GPO_OFF(BOOT0);
  
  USB_setup();
  __enable_irq();
  
  //uint32_t ntime_ms = 0;

  while(1){
    usb_class_poll();
    //GPO_T(RLED);
    /*if(systick_ms > ntime_ms){
      ntime_ms = systick_ms + 1000;
      ///GPO_T(GLED);
    }*/
    //sleep(1000000);
  }
}
