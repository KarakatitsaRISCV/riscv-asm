#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"

uint32_t SystemCoreClock = 8000000;

void uart_putc(char c){
  while(! (USART_STAT(USART0) & USART_STAT_TBE) ){}
  USART_DATA(USART0) = c;
}

void uart_b16(uint16_t val){
  for(uint16_t mask = 0x8000; mask!=0; mask>>=1){
    if(mask == 0x0080)uart_putc(' ');
    if(val & mask)uart_putc('1'); else uart_putc('0');
  }
  uart_putc('\r'); uart_putc('\n');
}

void sleep(uint32_t i){
  while(i--)asm volatile("nop");
}

uint16_t kbd_update(){
  uint16_t res = 0;
  
  GPIO_mode(A, 4, GPIO_PP50);
  sleep(10);
  res = GPIO_ISTAT(GPIOA) & 0x000F;
  
  GPIO_mode(A, 4, GPIO_HIZ); GPIO_mode(A, 5, GPIO_PP50);
  sleep(10);
  res <<= 4;
  res |= GPIO_ISTAT(GPIOA) & 0x000F;
  
  GPIO_mode(A, 5, GPIO_HIZ); GPIO_mode(A, 6, GPIO_PP50);
  sleep(10);
  res <<= 4;
  res |= GPIO_ISTAT(GPIOA) & 0x000F;
  
  GPIO_mode(A, 6, GPIO_HIZ); GPIO_mode(A, 7, GPIO_PP50);
  sleep(10);
  res <<= 4;
  res |= GPIO_ISTAT(GPIOA) & 0x000F;
    
  GPIO_mode(A, 7, GPIO_HIZ);
  return res;
}

int main(){
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  GPO_ON(GLED);
  
  USART_BAUD(USART0) = 8000000 / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_UEN;
  
  GPIO_CTL0(GPIOA) = (0x11110000 * GPIO_HIZ) | (0x00001111 * GPIO_PULL);
  GPIO_OCTL(GPIOA) = (GPIO_OCTL(GPIOA) & 0xFF00) | (0b1111 << 4);
  
  while(1){
    uint16_t res = kbd_update();
    uart_b16(res);
    
    GPO_T(YLED);
    sleep(100000);
  }
}
