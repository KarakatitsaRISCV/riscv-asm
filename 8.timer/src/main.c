#include <stdint.h>
#include "gd32vf103.h"
#include "interrupt_util.h"
#include "pinmacro.h"
#include "hardware.h"

uint32_t SystemCoreClock = 8000000;

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
}
void uart_puts(char *s){
  do{
    uart_putc(s[0]);
  }while(*(s++));
}

//#define TIMER_CTRL_ADDR           0xD1000000
#define MTIME_LO	(*(uint32_t*)(TIMER_CTRL_ADDR + 0x00))
#define MTIME_HI	(*(uint32_t*)(TIMER_CTRL_ADDR + 0x04))
#define MTIMECMP_LO	(*(uint32_t*)(TIMER_CTRL_ADDR + 0x08))
#define MTIMECMP_HI	(*(uint32_t*)(TIMER_CTRL_ADDR + 0x0C))
#define MTIMECTL	(*(uint32_t*)(TIMER_CTRL_ADDR + 0xFF8))

void mtime_init(){
  MTIMECMP_HI = 0;
  MTIMECMP_LO = 8000000 / 4 / 1;
  MTIME_LO = 0;
  MTIME_HI = 0;
  eclic_set_vmode( CLIC_INT_TMR );
  eclic_enable_interrupt( CLIC_INT_TMR );
}

void eclic_mtip_handler(){
  MTIME_LO = 0;
  MTIME_HI = 0;
}

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

#define TIME_INIT 1667772621
void RTC_init(){
  RCU_APB1EN |= RCU_APB1EN_PMUEN | RCU_APB1EN_BKPIEN;
  PMU_CTL |= PMU_CTL_BKPWEN;
  
  //if( !(RCU_BDCTL & RCU_BDCTL_RTCEN) ){
  if(1){
    RCU_BDCTL |= RCU_BDCTL_BKPRST;
    RCU_BDCTL &=~RCU_BDCTL_BKPRST;
    
    RCU_BDCTL |= RCU_BDCTL_RTCEN;
    RCU_BDCTL |= RCU_BDCTL_LXTALEN;
    uint32_t i;
    for(i=0; i<0x0FFF; i++){
      if(RCU_BDCTL & RCU_BDCTL_LXTALSTB){i=0xFFFF; break;}
    }
    if(i == 0xFFFF){ //LXTAL_OK
      i = 32768;
      RCU_BDCTL = (RCU_BDCTL &~RCU_BDCTL_RTCSRC) | RCU_RTCSRC_LXTAL;
    }else{ //LXTAL fail
      RCU_RSTSCK |= RCU_RSTSCK_IRC40KEN;
      for(i=0; i<0x0FFF; i++){
        if(RCU_RSTSCK & RCU_RSTSCK_IRC40KSTB){i=0xFFFF; break;}
      }
      if(i != 0xFFFF)return;
      RCU_BDCTL = (RCU_BDCTL &~RCU_BDCTL_RTCSRC) | RCU_RTCSRC_IRC40K;
      i = 40000;
    }
    
    RTC_CTL |= RTC_CTL_CMF;
    RTC_PSCH = 0;
    RTC_PSCL = i;
    RTC_CNTL = 0;
    RTC_CNTH = (TIME_INIT>>16)&0xFFFF;
    RTC_CNTL = (TIME_INIT)&0xFFFF;
    RTC_CTL &=~RTC_CTL_CMF;
    
    RTC_CTL &=~ RTC_CTL_RSYNF;
    while( !(RTC_CTL & RTC_CTL_RSYNF) ){}
  }else{
    RCU_RSTSCK |= RCU_RSTSCK_IRC40KEN;
    for(uint32_t i=0; i<0x0FFF; i++){
      if(RCU_RSTSCK & RCU_RSTSCK_IRC40KSTB){i=0xFFFF; break;}
    }
  }
}

uint32_t rtc_read(){
  uint16_t res_H1= RTC_CNTH;
  uint16_t res_L = RTC_CNTL;
  uint16_t res_H2= RTC_CNTH;
  if(res_H1 != res_H2)res_L = RTC_CNTL;
  
  return ((uint32_t)res_H2)<<16 | res_L;
}

int8_t clock_max(){
  int8_t res = 0; //HXTAL
  int i;
  uint32_t tmp;
  RCU_CTL &=~ RCU_CTL_HXTALEN;
  RCU_CTL &=~ RCU_CTL_HXTALSTB;
  RCU_CTL |= RCU_CTL_HXTALEN;
  for(i=0;i<0x0FFF;i++){
    if(RCU_CTL & RCU_CTL_HXTALSTB){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)res = 1; //IRC8M
  
  tmp = RCU_CFG0;
  tmp &=~(RCU_CFG0_PLLMF | RCU_CFG0_PLLSEL);
  if(res == 0){ //HXTAL, 8MHz
    tmp |= RCU_CFG0_PLLSEL;
    RCU_CFG1 = (RCU_CFG1 & RCU_CFG1_PREDV0) | RCU_PREDV0_DIV2; // 8/2 = 4 MHz
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
  if(i != 0x1FFF)return -1; //fail
  tmp = RCU_CFG0;
  tmp &=~RCU_CFG0_SCS;
  tmp |= RCU_CKSYSSRC_PLL;
  RCU_CFG0 = tmp;
  for(i=0;i<0x0FFF;i++){
    if((RCU_CFG0 & RCU_CFG0_SCSS)==RCU_SCSS_PLL){i=0x1FFF; break;}
  }
  if(i != 0x1FFF)return -1;
  SystemCoreClock = 108000000;
  return res;
}


typedef struct{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t day_week;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
}date_time_t;

void utc2date(date_time_t *res, uint32_t time){
  uint32_t a, b, c, d, e, m;
  uint32_t jd = 0;
  uint32_t jdn = 0;
  time += 3*60*60;
  jd = ((time+43200)/(86400>>1)) + (2440587<<1) + 1;
  jdn = jd>>1;
  res->sec = time % 60;
  time /= 60;
  res->min = time % 60;
  time /= 60;
  res->hour = time % 24;
  time /= 24;
  
  res->day_week = jdn % 7;
 
  a = jdn + 32044;
  b = (4*a+3)/146097;
  c = a - (146097*b)/4;
  d = (4*c+3)/1461;
  e = c - (1461*d)/4;
  m = (5*e+2)/153;
  res->day = e - (153*m+2)/5 + 1;
  res->month = m + 3 - 12*(m/10);
  res->year = 100*b + d - 4800 + (m/10);
}
const char *week_days[] = {
  "понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье"
};

#define TIM	B,6,1,GPIO_APP50

int main(){
  eclic_init( ECLIC_NUM_INTERRUPTS );
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  RCU_APB1EN |= RCU_APB1EN_TIMER3EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  
  USART_BAUD(USART0) = 8000000 / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_UEN;
  
  RTC_init();
  
  GPIO_config(TIM);
  TIMER_PSC(TIMER3) = (800 - 1);
  TIMER_CAR(TIMER3) = (100 - 1);
  TIMER_CH0CV(TIMER3) = 10;
  PM_BITMASK( TIMER_CHCTL0(TIMER3), TIMER_CHCTL0_CH0COMCTL, 0b110);
  PM_BITMASK( TIMER_CHCTL0(TIMER3), TIMER_CHCTL0_CH0MS, 0b00);
  TIMER_CHCTL2(TIMER3) |= TIMER_CHCTL2_CH0EN;
  TIMER_DMAINTEN(TIMER3) = TIMER_DMAINTEN_UPIE;
  
  TIMER_CTL0(TIMER3) |= TIMER_CTL0_CEN;
  eclic_set_vmode( TIMER3_IRQn );
  eclic_enable_interrupt( TIMER3_IRQn );

 
  uart_putc('>');
  
  eclic_global_interrupt_enable();
  
  while(1){
    uint32_t rtc = rtc_read();
    date_time_t dt;
    utc2date(&dt, rtc);
    
    uart_u32(dt.hour); uart_puts(":");
    uart_u32(dt.min); uart_puts(":");
    uart_u32(dt.sec); uart_puts("\t");
    uart_u32(dt.day); uart_puts(".");
    uart_u32(dt.month); uart_puts(".");
    uart_u32(dt.year); uart_puts("\t");
    uart_puts((char*)week_days[dt.day_week]); uart_puts("\r\n");
    
    
    TIMER_CH0CV(TIMER3)++;
    if(TIMER_CH0CV(TIMER3) > 100)TIMER_CH0CV(TIMER3) = 0;
    
    sleep(10000);
  }
}

void TIMER3_IRQHandler(){
  static uint16_t cnt = 0;
  cnt++;
  if(cnt > 100){
    GPO_T(RLED);
    cnt = 0;
  }
  TIMER_INTF(TIMER3) = 0;
}