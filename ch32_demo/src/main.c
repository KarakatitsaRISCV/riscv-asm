#include "ch32v30x.h"
#include "hardware.h"
#include "pinmacro.h"
#include "clock.h"
#define USART 1
#define UART_SIZE_PWR 8
#include "uart.h"

void SystemInit(void){}
void sleep(uint32_t t){while(t--)asm volatile("nop");}

void uart_u32(uint32_t val){
  char buf[11];
  char *ch = &buf[10];
  ch[0] = 0;
  do{
    *(--ch) = (val % 10) + '0';
    val /= 10;
  }while(val);
  while(ch[0] != 0){
    UART_putc(USART, ch[0]);
    ch++;
  }
  UART_putc(USART, '\r'); UART_putc(USART, '\n');
}
void uart_b16(uint16_t val){
  for(uint16_t mask = 0x8000; mask!=0; mask>>=1){
    if(mask == 0x0080)UART_putc(USART, ' ');
    if(val & mask)UART_putc(USART, '1') else UART_putc(USART, '0');
  }
  UART_putc(USART, '\r'); UART_putc(USART, '\n');
}


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



uint16_t kbd_update(){
  static uint16_t status_res = 0;
  static uint16_t status = 0;
  static uint16_t idx = 0;
  status <<= 4;
  status |= GPIOA->INDR & 0x000F;
  switch(idx){
    case 0:
      GPIO_mode(A, 4, GPIO_HIZ); GPIO_mode(A, 5, GPIO_PP50); idx = 1;
      break;
    case 1:
      GPIO_mode(A, 5, GPIO_HIZ); GPIO_mode(A, 6, GPIO_PP50); idx = 2;
      break;
    case 2:
      GPIO_mode(A, 6, GPIO_HIZ); GPIO_mode(A, 7, GPIO_PP50); idx = 3;
      break;
    default:
      GPIO_mode(A, 7, GPIO_HIZ); GPIO_mode(A, 4, GPIO_PP50); idx = 0;
      status_res = status;
      break;
  }
  return status_res;
}
void kbd_init(){
  GPIOA->CFGLR = (0x11110000 * GPIO_HIZ) | (0x00001111 * GPIO_PULL);
  GPIOA->OUTDR = (GPIOA->OUTDR & 0xFF00) | (0b1111 << 4);
}
void mask2disp(uint16_t mask){
  int res = 0;
  while(mask){
    res++;
    mask >>= 1;
  }
  if(res == 0){screen[10] = 0; return;}
  res--;
  screen[10] = seg_code[res];
}


extern const uint8_t adata_start[]   asm(AUDIOSRC "_start");
extern const uint8_t adata_end[]     asm(AUDIOSRC "_end");
volatile uint32_t data_size = 0;

void aud_init(){
  RCC->APB2PCENR |= RCC_TIM1EN;
  RCC->APB1PCENR |= RCC_TIM2EN;
  GPIO_config( PWM ); //TIM1.1
  data_size = adata_end - adata_start;
  
  TIM2->PSC = (1800 - 1); //144MHz / 1800 = 80000
  TIM2->ATRLR = (10 - 1);
  TIM2->CH1CVR = 1;
  TIM2->DMAINTENR = TIM_CC1E;
  TIM2->CTLR1 |= TIM_ARPE;
  NVIC_EnableIRQ( TIM2_IRQn );
  TIM2->CTLR1 |= TIM_CEN;
  
  TIM1->BDTR |= TIM_MOE;
  TIM1->PSC = (1-1);
  TIM1->ATRLR = (256 - 1);
  TIM1->CH1CVR = 128;
  PM_BITMASK( TIM1->CHCTLR1, TIM_OC1M, 0b110 ); //прямой ШИМ
  PM_BITMASK( TIM1->CHCTLR1, TIM_CC1S, 0b00 ); //output
  TIM1->CCER |= TIM_CC1E;
  PM_BITMASK( TIM1->CTLR1, TIM_CMS, 0b00 ); //edge-alignment mode
  TIM1->CTLR1 |= TIM_CEN;
}

__attribute__((interrupt)) void TIM2_IRQHandler(void){
  static uint32_t data_pos = 0;
  TIM1->CH1CVR = adata_start[ data_pos ];
  data_pos++;
  if(data_pos >= data_size){
    GPO_T(GLED);
    data_pos = 0;
  }
  
  TIM2->INTFR = 0;
}



int main(){
  clock_HS(1);
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED);
  GPO_OFF(RLED); GPO_OFF(GLED);
  UART_init(USART, 144000000/2/115200);
  UART_puts(USART, __TIME__ " " __DATE__ "\r\n");
  screen_init();
  kbd_init();
  aud_init();
  
  int led_t = 0;
  uint16_t kbd = 0, kbd_prev = 0;
  while(1){
    led_t++;
    if(led_t > 1000){
      //GPO_T(GLED);
      GPO_T(RLED);
      led_t = 0;
    }
    
    sleep(5000);
    screen_update();
    kbd = kbd_update();
    mask2disp(kbd);
    if(kbd != kbd_prev){
      uart_b16(kbd);
    }
    kbd_prev = kbd;
  }
}
