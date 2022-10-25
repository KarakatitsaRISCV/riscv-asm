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

#define ADDR_0	A,13,1,GPIO_PP50
#define ADDR_1	A,14,1,GPIO_PP50
#define ADDR_2	A,15,1,GPIO_PP50
#define ADDR_3	B,3,1,GPIO_PP50

void led_addr(uint8_t addr){
  if(addr & 1)GPO_ON(ADDR_0); else GPO_OFF(ADDR_0);
  if(addr & 2)GPO_ON(ADDR_1); else GPO_OFF(ADDR_1);
  if(addr & 4)GPO_ON(ADDR_2); else GPO_OFF(ADDR_2);
  if(addr & 8)GPO_ON(ADDR_3); else GPO_OFF(ADDR_3);
}

#define SEG_A	(1<<0)
#define SEG_B	(1<<1)
#define SEG_C	(1<<2)
#define SEG_D	(1<<3)
#define SEG_E	(1<<4)
#define SEG_F	(1<<5)
#define SEG_G	(1<<6)
#define SEG_DOT	(1<<7)

#define SEG_0	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define SEG_1	(SEG_B | SEG_C)
#define SEG_2	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)
#define SEG_3	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G)
#define SEG_4	(SEG_B | SEG_C | SEG_F | SEG_G)
#define SEG_5	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define SEG_6	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define SEG_7	(SEG_A | SEG_B | SEG_C)
#define SEG_8	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define SEG_9	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)

uint8_t screen[11] = {
  0b01100110,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00000000,
  0b10000001,
  0b01000010,
  0b00111100,
  
  0x00, 0xFF, 0x55
};

void screen_update(){
  static uint8_t cnt = 0;
  GPIO_OCTL(GPIOB) &= 0x00FF;
  led_addr(cnt);
  GPIO_OCTL(GPIOB) |= (screen[cnt] << 8);
  cnt++;
  if(cnt > 11)cnt = 0;
}

int main(){
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  GPO_ON(GLED);
  
  AFIO_PCF0 = (AFIO_PCF0 &~(0b111<<24)) | (0b100 << 24);
  GPIO_config(ADDR_0); GPIO_config(ADDR_1); GPIO_config(ADDR_2); GPIO_config(ADDR_3);
  GPIO_CTL1(GPIOB) = 0x11111111 * GPIO_PP50;
  
  while(1){
    screen[8] = SEG_5;
    screen[9] = SEG_6;
    screen[10]= SEG_7;
    screen_update();
  }
}
