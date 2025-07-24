#ifndef __UART_H__
#define __UART_H__

#if 1==0
ver.24.01.18

UART module for ch32

usage:
  #define USART 1 //USART1
  #define UART_SIZE_PWR 8 // 256-bytes ring buffer
  #include "uart.h"
  ...
  UART_init(USART, 8000000/9600); //Init USART1 (by macro USART)
  UART_puts(USART, __TIME__ " " __DATE__ "\r\n");

usage in another *.c files:
  #define UART_DECLARATIONS 1 //USART1
  #include "uart.h"
  ...
  UART_puts(USART, __TIME__ " " __DATE__ "\r\n");
  

модуль для работы с UART в ch32

макронастройки:
  USART - Номер UART. 1, 2, ...
  
макросы и функции:
  void UART_init(int num, uint32_t brr)
  
  void UART_putc(int num, uint8_t data)
  void UART_write(int num, uint8_t data, uint8_t len)
  void UART_puts(int num, char* str)
  uint8_t UART_tx_count(int num)
  
  uint8_t UART_getc(int num)
  void UART_read(int num, uint8_t data, uint8_t len)
  void UART_gets(int num, char* str, uint8_t len)
  uint8_t UART_scan(int num)
  uint8_t UART_rx_count(int num)
где
  num - номер UART (1 - 3)
  data - байт данных для приема / передачи
  str - строка для приема / передачи
  len - (максимальное) количество байт для приема / передачи
#endif
//TODO: настроить ремап портов

#if !defined(USART) && !defined(UART_DECLARATIONS)
  #error define either USART or UART_DECLARATIONS macro
#endif


#define _UART_PIN(num,dir) UART ## num ## _ ## dir
#define UART_PIN(num,dir) _UART_PIN(num,dir)
#define _uartN_Dx(num, dir) uart ## num ## _ ## dir
#define uartN_Dx(num, dir) _uartN_Dx(num, dir)
#define _UART(num) USART ## num
#define UART(num) _UART(num)
#define _UART_IRQ(num) USART##num##_IRQn
#define UART_IRQ(num) _UART_IRQ(num)
#define _RCC_USARTnEN(num) RCC_USART ## num ## EN
#define RCC_USARTnEN(num) _RCC_USARTnEN(num)

#define _UART_init(num, brr) UART ## num ## _init(brr)
#define _UART_rx_count(num)	((UART_SIZE - uart_buf_size(&_uartN_Dx(num,rx))) & UART_MASK )
#define _UART_tx_count(num)  (UART_MASK - uart_buf_size(&_uartN_Dx(num,tx)))
#define _UART_getc(num)  	uart_buf_read(&_uartN_Dx(num,rx))
#define _UART_scan(num)      (_uartN_Dx(num,rx).arr[_uartN_Dx(num,rx).st])
#define _UART_write(num, data, len) UART##num##_write(data, len)
#define _UART_puts(num, str) UART##num##_puts(str)
#define _UART_read(num, data, len) UART##num##_read(data, len)
#define _UART_gets(num, str, len) UART##num##_gets(str, len)

#define UART_init(num, brr) _UART_init(num, brr)
#define UART_rx_count(num)	_UART_rx_count(num)
#define UART_tx_count(num)  _UART_tx_count(num)
#define UART_getc(num)  	_UART_getc(num)
#define UART_scan(num)      _UART_scan(num)
#define UART_write(num, data, len) _UART_write(num, data, len)
#define UART_puts(num, str) _UART_puts(num, str)
#define UART_read(num, data, len) _UART_read(num, data, len)
#define UART_gets(num, str, len) _UART_gets(num, str, len)

#define _UARTn_func(n, func) UART ## n ## _ ## func
#define UARTn_func(n, func) _UARTn_func(n, func)

#define UART_putc(num, data) do{\
  uart_buf_write(&_uartN_Dx(num,tx), data);\
  UART(num)->CTLR1 |= USART_CTLR1_TXEIE;\
}while(0);




#ifndef UART_DECLARATIONS

#include "pinmacro.h"

#ifndef UART_SIZE_PWR
  #define UART_SIZE_PWR 6
#endif

#define UART_SIZE (1<<UART_SIZE_PWR)
#define UART_MASK (UART_SIZE-1)

typedef struct{
  volatile uint8_t st,en;
  volatile uint8_t arr[UART_SIZE];
}uart_buffer;

uart_buffer uartN_Dx(USART, rx); //uart1_rx
uart_buffer uartN_Dx(USART, tx); //uart1_tx

uint8_t uart_buf_size(uart_buffer *buf){
  return ((buf->st - buf->en) & UART_MASK);
}

uint8_t uart_buf_read(uart_buffer *buf){
  uint8_t res;
  if(uart_buf_size(buf) == 0)return 0;
  res = buf->arr[buf->st];
  buf->st++;
  buf->st &= UART_MASK;
  return res;
}

void uart_buf_write(uart_buffer *buf, uint8_t dat){
  if(uart_buf_size(buf)!=1){
    buf->arr[buf->en]=dat;
    buf->en++; buf->en &= UART_MASK;
  }
}

//UART1_write
void UARTn_func(USART, write)(uint8_t *data, uint8_t len){
  while(len--)UART_putc(USART, *(data++));
}

//UART1_puts
void UARTn_func(USART, puts)(char *str){
  while(str[0] != 0)UART_putc(USART, *(str++));
}

//UART1_read
void UARTn_func(USART, read)(uint8_t *data, uint8_t len){
  while(len--){
    while(UART_rx_count(USART) == 0){}
    *(data++) = UART_getc(USART);
  }
}

//UART1_gets
void UARTn_func(USART, gets)(char *str, uint8_t len){
  while(len--){
    while(UART_rx_count(USART) == 0){}
    str[0] = UART_getc(USART);
    if(str[0] == 0 || str[0] == 13)break;
    str++;
  }
  if(str[0] != 0){
    if(len < 3)str[0] = 0;
      else{ str[0] = 0x0A; str[1] = 0x0D; str[2] = 0; }
  }
}

#define UART1_TX A,9 ,1,GPIO_APP50
#define UART1_RX A,10,1,GPIO_HIZ

#define UART2_TX A,2 ,1,GPIO_APP50
#define UART2_RX A,3 ,1,GPIO_HIZ

#define UART3_TX B,10,1,GPIO_APP50
#define UART3_RX B,11,1,GPIO_HIZ

//UART1_init
void UARTn_func(USART, init)(uint16_t brr){
  GPIO_config( UART_PIN(USART, RX) );
  GPIO_config( UART_PIN(USART, TX) );
#if USART == 1
  RCC->APB2PCENR |= RCC_USART1EN;
#else
  RCC->APB1PCENR |= RCC_USARTnEN(USART);
#endif
  UART(USART)->BRR = (brr);
  UART(USART)->CTLR1 = USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE | USART_CTLR1_RXNEIE;
  UART(USART)->CTLR2 = 0;
  UART(USART)->CTLR3 = 0;
  UART(USART)->GPR = 0;
  uartN_Dx(USART,rx).st=0; uartN_Dx(USART,rx).en=0; uartN_Dx(USART,tx).st=0; uartN_Dx(USART,tx).en=0;
  NVIC_EnableIRQ( UART_IRQ(USART) );
}

///////////////////////////////////////////////////////////////////////////////////////////////
//             Interrupt
///////////////////////////////////////////////////////////////////////////////////////////////
#define _uart_IRQ(num) USART ## num ## _IRQHandler
#define uart_IRQ(num) _uart_IRQ(num)
//__attribute__((interrupt)) void USART2_IRQHandler(void){
__attribute__((interrupt)) void uart_IRQ(USART)(void){
  if( UART(USART)->STATR & USART_STATR_RXNE ){
    uint8_t temp = UART(USART)->DATAR;
    uart_buf_write(&uartN_Dx(USART, rx), temp);
  }else if( UART(USART)->STATR & USART_STATR_TXE ){
    if(uart_buf_size(&uartN_Dx(USART, tx)) != 0)UART(USART)->DATAR = uart_buf_read(&uartN_Dx(USART, tx));
      else UART(USART)->CTLR1 &=~ USART_CTLR1_TXEIE;
  }
}

#else //UART_DECLARATIONS

//UART1_write
void UARTn_func(UART_DECLARATIONS, write)(uint8_t *data, uint8_t len);
//UART1_puts
void UARTn_func(UART_DECLARATIONS, puts)(char *str);
//UART1_read
void UARTn_func(UART_DECLARATIONS, read)(uint8_t *data, uint8_t len);
//UART1_gets
void UARTn_func(UART_DECLARATIONS, gets)(char *str, uint8_t len);
//UART1_init
void UARTn_func(UART_DECLARATIONS, init)(uint16_t brr);

#endif

#endif
