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


char mem_src[4052] = "Встраиваемая система (англ. embedded system) — специализированная микропроцессорная система управления, контроля и мониторинга, концепция разработки которой заключается в том, что такая система будет работать, будучи встроенной непосредственно в устройство, которым она управляет (определение из Википедии). Как следует из определения, такая система обычно не обладает “стандартными” средствами ввода-вывода - монитором, клавиатурой, мышкой. Вместо этого она взаимодействует с окружающей средой при помощи самых разнообразных датчиков, кнопочек, исполнительных механизмов и индикаторов. Например, встроенная в стиральную машину система может уметь управлять асинхронным мотором (несколько линий ШИМ, датчики Холла, измерители тока), подогревом воды (управление реле нагревателя, датчик температуры), оценивать массу заложенного белья (датчик нагрузки или ускорения), управлять водяным клапаном, позволять пользователю выбирать режим работы (“крутилка” энкодер, кнопки, знаковый дисплей). При этом важным является не умение проводить сложные расчеты, не умение запускать множество интерактивных программ одновременно, а именно взаимодействие с периферией и быстрая реакция на внешние события. Если в процессе работы двигатель по какой-то причине замедлился, надо срочно подстраивать управляющие сигналы, а то и останавливать его, если что-то пошло совсем не так. В ряде случаев роль играют такие параметры, как энерноэффективность и занимаемый объем. Поскольку подобные системы обычно предоставляются пользователю в виде законченного монолитного устройства, то и схемотехнику, и софт также обычно делают монолитными. Вместо кучи драйверов под все существующее оборудование, жестко прописывают только то, которое реально установлено на плате. Вместо возможности запуска сторонних программ со всеми защитами и разделением прав доступа, сразу закладывают весь возможный функционал без возможности расширения. В конце концов, кому придет в голову вскрывать корпус той же стиральной машины чтобы подпаять туда дисплей и любоваться графикой?! Хотя умельцы, конечно, находятся… Отсюда вытекает желание производителей уместить на одном чипе не только вычислительное ядро (процессор), но и память, и периферию.\r\n";

char mem_dst[4052] = "Initial data\r\n";

uint32_t gpio_data[2] = {(1<<6), (1<<6<<16)};

int main(){
  eclic_init( ECLIC_NUM_INTERRUPTS );
  RCU_APB2EN |= RCU_APB2EN_PAEN | RCU_APB2EN_PBEN | RCU_APB2EN_AFEN | RCU_APB2EN_USART0EN;
  RCU_APB1EN |= RCU_APB1EN_TIMER3EN;
  
  GPIO_config(USART_TX); GPIO_config(USART_RX);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPIO_config(RBTN); GPIO_config(SBTN);
  
  USART_BAUD(USART0) = 8000000 / 9600;
  USART_CTL0(USART0) = USART_CTL0_TEN | USART_CTL0_REN | USART_CTL0_UEN;
  
  uart_putc('>');
  
  TIMER_PSC(TIMER3) = (8000 - 1);
  TIMER_CAR(TIMER3) = (1000 - 1);
  TIMER_DMAINTEN(TIMER3) = TIMER_DMAINTEN_UPIE;
  
  TIMER_CTL0(TIMER3) |= TIMER_CTL0_CEN;
  eclic_set_vmode( TIMER3_IRQn );
  eclic_enable_interrupt( TIMER3_IRQn );
  
  eclic_global_interrupt_enable();
  
  
  RCU_AHBEN |= RCU_AHBEN_DMA0EN;
  
  //DMA0.6 - blink yellow LED
  DMA_CHCTL(DMA0, 6) = 0;
  DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_CHINTF_RESET_VALUE, 6);
  DMA_CHMADDR(DMA0, 6) = (uint32_t)gpio_data;
  DMA_CHPADDR(DMA0, 6) = (uint32_t)&GPIO_BOP(GPIOB);
  DMA_CHCNT(DMA0, 6) = 2;
  
  DMA_CHCTL(DMA0, 6) = DMA_PERIPHERAL_WIDTH_32BIT | DMA_MEMORY_WIDTH_32BIT | DMA_CHXCTL_MNAGA | DMA_CHXCTL_CMEN | DMA_CHXCTL_DIR;
  DMA_CHCTL(DMA0, 6) |= DMA_CHXCTL_CHEN;
  
  TIMER_DMAINTEN(TIMER3) |= TIMER_DMAINTEN_UPDEN;
  
  //DMA0.1 - copy from mem_src to mem_dst
  DMA_CHCTL(DMA0, 1) = 0;
  DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_CHINTF_RESET_VALUE, 1);
  DMA_CHMADDR(DMA0, 1) = (uint32_t)mem_src;
  DMA_CHPADDR(DMA0, 1) = (uint32_t)mem_dst;
  DMA_CHCNT(DMA0, 1) = sizeof(mem_src);
  DMA_CHCTL(DMA0, 1) = DMA_CHXCTL_M2M | DMA_PERIPHERAL_WIDTH_8BIT | DMA_MEMORY_WIDTH_8BIT | DMA_CHXCTL_MNAGA | DMA_CHXCTL_PNAGA  | DMA_CHXCTL_DIR;
  DMA_CHCTL(DMA0, 1) |= DMA_CHXCTL_CHEN;
  
  while( !(DMA_INTF(DMA0) & DMA_FLAG_ADD(DMA_INTF_FTFIF, 1)) ){}
  
  //DMA0.3 0 output from mem_dst to UART
  DMA_CHCTL(DMA0, 3) = 0;
  DMA_INTC(DMA0) = DMA_FLAG_ADD(DMA_CHINTF_RESET_VALUE, 3);
  DMA_CHMADDR(DMA0, 3) = (uint32_t)mem_dst;
  DMA_CHPADDR(DMA0, 3) = (uint32_t)&USART_DATA(USART0);
  DMA_CHCNT(DMA0, 3) = sizeof(mem_dst);
  DMA_CHCTL(DMA0, 3) = DMA_PERIPHERAL_WIDTH_8BIT | DMA_MEMORY_WIDTH_8BIT | DMA_CHXCTL_MNAGA | DMA_CHXCTL_DIR;
  DMA_CHCTL(DMA0, 3) |= DMA_CHXCTL_CHEN;
  
  USART_CTL2(USART0) = USART_CTL2_DENT;
  
  //--------------------------------------------------------
  
  while(1){
    GPO_T(GLED);
    sleep(1000000);
  }
}

void TIMER3_IRQHandler(){
  GPO_T(RLED);
  TIMER_INTF(TIMER3) = 0;
}
