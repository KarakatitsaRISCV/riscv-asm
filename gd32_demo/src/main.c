#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"

uint32_t SystemCoreClock = 8000000;
//#define F_CPU	8000000
#define F_CPU	108000000

void sleep(uint32_t i){
  while(i--)asm volatile("nop");
}

void uart_putc(char c){
  while(! (USART_STAT(USART0) & USART_STAT_TBE) ){}
  USART_DATA(USART0) = c;
}
void uart_u32(uint32_t val){
  char buf[11];
  char *ch = &buf[10];
  ch[0] = 0;
  do{
    *(--ch) = (val % 10) + '0';
    val /= 10;
  }while(val);
  while(ch[0] != 0){
    uart_putc(ch[0]);
    ch++;
  }
  uart_putc('\r'); uart_putc('\n');
}
void uart_b16(uint16_t val){
  for(uint16_t mask = 0x8000; mask!=0; mask>>=1){
    if(mask == 0x0080)uart_putc(' ');
    if(val & mask)uart_putc('1'); else uart_putc('0');
  }
  uart_putc('\r'); uart_putc('\n');
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
  GPIO_OCTL(GPIOB) &= 0x00FF;
  led_addr(cnt);
  GPIO_OCTL(GPIOB) |= (screen[cnt] << 8);
  cnt++;
  if(cnt > 11)cnt = 0;
}

void screen_init(){
  AFIO_PCF0 = (AFIO_PCF0 &~(0b111<<24)) | (0b100 << 24);
  GPIO_config(ADDR_0); GPIO_config(ADDR_1); GPIO_config(ADDR_2); GPIO_config(ADDR_3);
  GPIO_CTL1(GPIOB) = 0x11111111 * GPIO_PP50;
}



uint16_t kbd_update(){
  static uint16_t status_res = 0;
  static uint16_t status = 0;
  static uint16_t idx = 0;
  status <<= 4;
  status |= GPIO_ISTAT(GPIOA) & 0x000F;
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
  GPIO_CTL0(GPIOA) = (0x11110000 * GPIO_HIZ) | (0x00001111 * GPIO_PULL);
  GPIO_OCTL(GPIOA) = (GPIO_OCTL(GPIOA) & 0xFF00) | (0b1111 << 4);
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



#define CLOCK_FAIL -1
#define CLOCK_HSE 2
#define CLOCK_HSI 1
int8_t clock_max(){
  int8_t res = CLOCK_HSE;
  int i;
  uint32_t tmp;
  RCU_CTL &=~ RCU_CTL_HXTALEN;
  RCU_CTL &=~ RCU_CTL_HXTALSTB;
  RCU_CTL |= RCU_CTL_HXTALEN;
  for(i=0;i<0x0FFF;i++){
    if(RCU_CTL & RCU_CTL_HXTALSTB){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)res = CLOCK_HSI;
  
  tmp = RCU_CFG0;
  tmp &=~(RCU_CFG0_PLLMF | RCU_CFG0_PLLSEL);
  if(res == CLOCK_HSE){ //8MHz
    tmp |= RCU_CFG0_PLLSEL;
    RCU_CFG1 = (RCU_CFG1 & RCU_CFG1_PREDV0) | RCU_PREDV0_DIV2; //8/2 = 4 MHz
  }
  tmp |= RCU_PLL_MUL27; //CK_SYS = 4*27 = 108 MHz
  
  tmp &=~ (RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC);
  tmp |= RCU_AHB_CKSYS_DIV1; //AHB = CK_SYS/1 = 108 MHz
  tmp |= RCU_APB1_CKAHB_DIV2; //APB1 = AHB/2 = 54 MHz
  tmp |= RCU_APB2_CKAHB_DIV1; //APB2 = AHB/1 = 108 MHz
  
  RCU_CFG0 = tmp;
  
  RCU_CTL |= RCU_CTL_PLLEN;
  for(i=0;i<0x0FFF;i++){
    if(RCU_CTL & RCU_CTL_PLLSTB){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)return CLOCK_FAIL;
  tmp = RCU_CFG0;
  tmp &=~RCU_CFG0_SCS;
  tmp |= RCU_CKSYSSRC_PLL;
  RCU_CFG0 = tmp;
  for(i=0;i<0x0FFF;i++){
    if((RCU_CFG0 & RCU_CFG0_SCSS)==RCU_SCSS_PLL){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)return CLOCK_FAIL;
  SystemCoreClock = 108000000;
  return res;
}

//datasheet, p.33
//refman, p.222
//PB6 = TIM3.0
//PB8 = TIM3.2 -- в качестве ДЗ будет вывод на PA8 (TIM0.0)
//SBTN = TIM2.2

#define TIM	A,8,1,GPIO_APP50 //TIM0.0

inline uint32_t read_mcycles(){
  uint32_t res;
  asm volatile("csrr %0, mcycle" : "=r"(res) );
  return res;
}

void delay_cycles(uint32_t cycles){
  int32_t cur_cycles, av_cycles;
  av_cycles = read_mcycles() + cycles;
  do{cur_cycles = read_mcycles();}while( (av_cycles - cur_cycles) > 0 );
}

void delay_us(uint32_t us){
  int32_t cur_cycles, av_cycles;
  av_cycles = read_mcycles() + (us * (F_CPU / 1000000));
  do{cur_cycles = read_mcycles();}while( (av_cycles - cur_cycles) > 0 );
}

void delay_ms(uint32_t ms){
  int32_t cur_cycles, av_cycles;
  av_cycles = read_mcycles() + (ms * (F_CPU / 1000 ));
  do{cur_cycles = read_mcycles();}while( (av_cycles - cur_cycles) > 0 );
}

extern const uint8_t adata_start[]   asm(AUDIOSRC "_start");
extern const uint8_t adata_end[]     asm(AUDIOSRC "_end");

int main(){
#if F_CPU == 108000000
  clock_max();
#endif
  eclic_init( ECLIC_NUM_INTERRUPTS );
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  RCU_APB2EN |= RCU_APB2EN_TIMER0EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  
  screen_init();
  kbd_init();
  
  GPIO_config(TIM); //TODO: важно!
  
  //USART_BAUD(USART0) = 8000000 / 9600;
  USART_BAUD(USART0) = F_CPU / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_UEN;
  uart_putc('>');
  
  TIMER_PSC(TIMER0) = (1 - 1);
  TIMER_CAR(TIMER0) = (256 - 1); // 31250 Hz
  
  TIMER_CH0CV(TIMER0) = 128;
  TIMER_CCHP(TIMER0) = TIMER_CCHP_POEN; //ВАЖНО!
  PM_BITMASK( TIMER_CHCTL0(TIMER0), TIMER_CHCTL0_CH0COMCTL, 0b110 ); //прямой ШИМ
  PM_BITMASK( TIMER_CHCTL0(TIMER0), TIMER_CHCTL0_CH0MS, 0b00 );
  TIMER_CHCTL2(TIMER0) |= TIMER_CHCTL2_CH0EN;
  PM_BITMASK( TIMER_CTL0(TIMER0), TIMER_CTL0_CAM, 0b00 );
  TIMER_CTL0(TIMER0) = TIMER_CTL0_CEN;
  
  
  eclic_global_interrupt_enable();
  
  uint32_t cyc_prev = 0, cyc_cur;
  uint32_t i = 0;
  uint32_t data_size = (adata_end - adata_start);
  uint16_t kbd = 0, kbd_prev = 0;
  while(1){
    TIMER_CH0CV(TIMER0) = adata_start[i];
    i++;
    if(i >= data_size){
      i = 0;
      GPO_T(YLED);
    }
    delay_us(1000000 / 8000);
    GPO_T(RLED);
    screen_update();
    
    kbd = kbd_update();
    mask2disp(kbd);
    if(kbd != kbd_prev){
      uart_b16(kbd);
    }
    kbd_prev = kbd;
    
  }
}


