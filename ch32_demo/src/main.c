#include "ch32v30x.h"
#include "hardware.h"
#include "pinmacro.h"
#include "clock.h"
#define USART 1
#define UART_SIZE_PWR 8
#include "uart.h"

void SystemInit(void){}
void sleep(uint32_t t){while(t--)asm volatile("nop");}




#define ADDR_0	B,0,1,GPIO_PP50
#define ADDR_1	B,1,1,GPIO_PP50
#define ADDR_2	B,2,1,GPIO_PP50
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

#define NUM_0	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define NUM_1	(SEG_B | SEG_C)
#define NUM_2	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)
#define NUM_3	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G)
#define NUM_4	(SEG_B | SEG_C | SEG_F | SEG_G)
#define NUM_5	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define NUM_6	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define NUM_7	(SEG_A | SEG_B | SEG_C)
#define NUM_8	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define NUM_9	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)
#define NUM_A	(SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define NUM_B	(SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define NUM_C	(SEG_A | SEG_D | SEG_E | SEG_F)
#define NUM_D	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)
#define NUM_E	(SEG_A | SEG_D | SEG_E | SEG_F | SEG_G)
#define NUM_F	(SEG_A | SEG_E | SEG_F | SEG_G)
const uint8_t seg_code[] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_A, NUM_B, NUM_C, NUM_D, NUM_E, NUM_F};

uint8_t screen[11] = {
  0b01100110,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00000000,
  0b10000001,
  0b01000010,
  0b00111100,
  
  NUM_0, NUM_0, NUM_0
};

void screen_update(){
  static uint8_t cnt = 0;
  GPIOB->OUTDR &= 0x00FF;
  led_addr(cnt);
  GPIOB->OUTDR |= (screen[cnt] << 8);
  cnt++;
  if(cnt > 11)cnt = 0;
}

void screen_init(){
  GPIO_config(ADDR_0); GPIO_config(ADDR_1); GPIO_config(ADDR_2); GPIO_config(ADDR_3);
  GPIOB->CFGHR = 0x11111111 * GPIO_PP50;
}




int main(){
  clock_HS(1);
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED);
  GPO_OFF(RLED); GPO_OFF(GLED);
  UART_init(USART, 144000000/2/115200);
  UART_puts(USART, __TIME__ " " __DATE__ "\r\n");
  screen_init();
  
  int led_t = 0;
  while(1){
    led_t++;
    if(led_t > 1000){
      GPO_T(GLED);
      GPO_T(RLED);
      led_t = 0;
    }
    
    sleep(5000);
    screen_update();
  }
}
