#ifndef __UART_H__
#define __UART_H__

#if 1==0
ver.24.05.21

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

Macro-settings:
  USART - UART number: 1, 2, ...
  USART_REMAP - remap number: 0(default), 1, 2, ...
  

// num - UART number (usually, USART macro)
// Initialize UART. brr - baudrate
void UART_init(num, brr);

// Return count of bytes received
uint32_t UART_received(num);

// Return length of received string (string end is \r or \n)
//  return value -1: buffer overflow
//  return value 0: string is not received (\r or \n not found)
//  return value other: length of string
int32_t UART_str_size(num);

// Return count of bytes avaible to send buffer
uint32_t UART_avaible(num);

// Receive 1 byte (return -1 - receive buffer is empty)
int16_t UART_getc(num);

// Receive [len] bytes into [data]
//  return value NULL: bytes received less than [len]
//  return value other: [data]
char* UART_read(num, data, len)

// Receive string with maximum length [len] into [str]
//  return value NULL: string is not received
//  return value other: [str]
char* UART_gets(num, str, len);

// Clear receive buffer
void UART_rx_clear(num);

// Send [len] bytes from [data]
void UART_write(num, data, len);

// Send string [str]
void UART_puts(num, str);
  

#endif

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define UART_init(num, brr) _UART_init(num, brr)
#define UART_received(num)	_UART_received(num)
#define UART_str_size(num) _UART_str_size(num) //-1=error, 0=\r\n not found, other = string length
#define UART_avaible(num)  _UART_avaible(num)
#define UART_getc(num)  	_UART_getc(num)
#define UART_read(num, data, len) _UART_read(num, data, len)
#define UART_gets(num, str, len) _UART_gets(num, str, len)
#define UART_rx_clear(num) _UART_rx_clear(num)
#define UART_write(num, data, len) _UART_write(num, data, len)
#define UART_puts(num, str) _UART_puts(num, str)

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
#define _UART_received(num)	(uart_buf_busy(&_uartN_Dx(num,rx)))
#define _UART_avaible(num)  (uart_buf_free(&_uartN_Dx(num,tx)))
#define _UART_str_size(num) (uart_buf_str_size( &_uartN_Dx(num,rx) ) )
#define _UART_getc(num)  	uart_buf_getc(&_uartN_Dx(num,rx))
#define _UART_read(num, data, len) UART##num##_read(data, len)
#define _UART_gets(num, str, len) uart_buf_gets(&_uartN_Dx(num,rx), str, len)
#define _UART_rx_clear(num) do{_uartN_Dx(num,rx).en = _uartN_Dx(num,rx).st;}while(0)
#define _UART_write(num, data, len) UART##num##_write(data, len)
#define _UART_puts(num, str) UART##num##_puts(str)

#define _UARTn_func(n, func) UART ## n ## _ ## func
#define UARTn_func(n, func) _UARTn_func(n, func)

#define UART_putc(num, data) do{\
  uart_buf_putc(&_uartN_Dx(num,tx), data);\
  UART(num)->CTLR1 |= USART_CTLR1_TXEIE;\
}while(0)

#include <stdint.h>


#ifdef USART


#include "pinmacro.h"

#ifndef UART_SIZE_PWR
  #define UART_SIZE_PWR 6
#endif

#define UART_SIZE (1<<UART_SIZE_PWR)
#define UART_MASK (UART_SIZE-1)

typedef struct{
  volatile uint32_t st,en;
  volatile uint8_t arr[UART_SIZE];
}uart_buffer;

uart_buffer uartN_Dx(USART, rx); //uart1_rx
uart_buffer uartN_Dx(USART, tx); //uart1_tx

static uint32_t uart_buf_busy(uart_buffer *buf){
  return ((buf->st - buf->en) & UART_MASK);
}
//#define uart_buf_busy(buf) (((buf)->st - (buf)->en) & UART_MASK)
#define uart_buf_free(buf) (UART_SIZE - uart_buf_busy(buf) - 1)

static int16_t uart_buf_getc(uart_buffer *buf){
  uint8_t res;
  if(uart_buf_busy(buf) == 0)return -1;
  res = buf->arr[buf->en];
  buf->en++;
  buf->en &= UART_MASK;
  return res;
}

static void uart_buf_putc(uart_buffer *buf, uint8_t dat){
  if(uart_buf_free(buf) < 1)return;
  buf->arr[buf->st]=dat;
  buf->st++;
  buf->st &= UART_MASK;
}

static int32_t uart_buf_str_size(uart_buffer *buf){
  uint32_t en = buf->en;
  uint8_t *arr = (uint8_t*)(buf->arr);
  uint32_t sz = (buf->st - en) & UART_MASK;
  uint32_t pos;
  if(sz == UART_SIZE - 1)return -1;
  for(uint32_t i=1; i<sz; i++){
    pos = (i + en) & UART_MASK;
    if((arr[pos] == '\r') || (arr[pos] == '\n'))return i;
  }
  return 0;
}

static char *uart_buf_gets(uart_buffer *buf, char *str, uint32_t len){
  uint32_t en = buf->en;
  uint8_t *arr = (uint8_t*)(buf->arr);
  uint32_t sz = (buf->st - en) & UART_MASK;
  uint32_t strsz;
  uint32_t pos = (1 + en) & UART_MASK;
  str[0] = 0;
  if(sz > len)sz = len;
  if(sz < 2)return NULL;
  for(strsz = 1; strsz < sz; strsz++){ // find '\r\n' in buffer
    pos = (strsz + en) & UART_MASK;
    if((arr[pos] == '\r') || (arr[pos] == '\n'))break;
  }
  if((arr[pos] != '\r') && (arr[pos] != '\n')){ // '\r\n' not found
    if(strsz < len)return NULL; // if user requests less bytes then return received part
  }
  
  if((arr[en] == '\r') || (arr[en] == '\n')){ //remove '\r\n' from start
    en = (en+1)&UART_MASK;
    strsz--;
  }
  
  for(uint32_t i=0; i<strsz; i++){
    str[i] = arr[ (i+en)&UART_MASK ];
  }
  str[strsz] = 0;
  
  buf->en = (strsz+en) & UART_MASK;
  return str;
}

//UART1_write
void UARTn_func(USART, write)(uint8_t *data, uint32_t len){
  while(len--){
    uart_buf_putc(&uartN_Dx(USART,tx), data[0]);
    data++;
  }
  UART(USART)->CTLR1 |= USART_CTLR1_TXEIE;
}

//UART1_puts
void UARTn_func(USART, puts)(char *str){
  while(str[0] != 0){
    uart_buf_putc(&uartN_Dx(USART,tx), str[0]);
    str++;
  }
  UART(USART)->CTLR1 |= USART_CTLR1_TXEIE;
}

//UART1_read
uint8_t* UARTn_func(USART, read)(uint8_t *data, uint32_t len){
  data[0] = 0;
  if(UART_received(USART) < len)return NULL;
  while(len--){
    *(data++) = UART_getc(USART);
  }
  return data;
}

#ifndef USART_REMAP
  #define USART_REMAP 0
#endif

//спасибо ленивым китайцам, которые не прописали это в инклюднике!
#define AFIO_PCFR2_USART1_RM1	(1LLU << 26) 
#define AFIO_PCFR2_USART4_RM	(1LLU << 16)
#define AFIO_PCFR2_USART5_RM	(1LLU << 18)
#define AFIO_PCFR2_USART6_RM	(1LLU << 20)
#define AFIO_PCFR2_USART7_RM	(1LLU << 22)
#define AFIO_PCFR2_USART8_RM	(1LLU << 24)

#if USART == 1
  #if USART_REMAP == 0
    #define UART1_TX A,9 ,1,GPIO_APP50
    #define UART1_RX A,10,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART1_REMAP ) | 0*AFIO_PCFR1_USART1_REMAP; \
      AFIO->PCFR2 = (AFIO->PCFR2 &~AFIO_PCFR2_USART1_RM1) | 0*AFIO_PCFR2_USART1_RM1; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART1_TX B,6,1,GPIO_APP50
    #define UART1_RX B,7,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART1_REMAP ) | 1*AFIO_PCFR1_USART1_REMAP; \
      AFIO->PCFR2 = (AFIO->PCFR2 &~AFIO_PCFR2_USART1_RM1) | 0*AFIO_PCFR2_USART1_RM1; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART1_TX B,15,1,GPIO_APP50
    #define UART1_RX A,8, 1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART1_REMAP ) | 0*AFIO_PCFR1_USART1_REMAP; \
      AFIO->PCFR2 = (AFIO->PCFR2 &~AFIO_PCFR2_USART1_RM1) | 1*AFIO_PCFR2_USART1_RM1; \
    }while(0)
  #elif USART_REMAP == 3
    #define UART1_TX A,6,1,GPIO_APP50
    #define UART1_RX A,7,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART1_REMAP ) | 1*AFIO_PCFR1_USART1_REMAP; \
      AFIO->PCFR2 = (AFIO->PCFR2 &~AFIO_PCFR2_USART1_RM1) | 1*AFIO_PCFR2_USART1_RM1; \
    }while(0)
  #else
    #error USART1 remap must be defined as 0 (no remap), 1, 2 or 3  
  #endif
#endif

#if USART == 2
  #if USART_REMAP == 0
    #define UART2_TX A,2 ,1,GPIO_APP50
    #define UART2_RX A,3 ,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART2_REMAP ) | 0*AFIO_PCFR1_USART2_REMAP; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART2_TX D,5 ,1,GPIO_APP50
    #define UART2_RX D,6 ,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART2_REMAP ) | 1*AFIO_PCFR1_USART2_REMAP; \
    }while(0)
  #else
    #error USART2 remap must be defined as 0 or 1
  #endif
#endif

#if USART == 3
  #if USART_REMAP == 0
    #define UART3_TX B,10,1,GPIO_APP50
    #define UART3_RX B,11,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART3_REMAP ) | 0*AFIO_PCFR1_USART3_REMAP_0; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART3_TX C,10,1,GPIO_APP50
    #define UART3_RX C,11,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART3_REMAP ) | 1*AFIO_PCFR1_USART3_REMAP_0; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART3_TX A,13,1,GPIO_APP50
    #define UART3_RX A,14,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART3_REMAP ) | 2*AFIO_PCFR1_USART3_REMAP_0; \
    }while(0)
  #elif USART_REMAP == 3
    #define UART3_TX D,8,1,GPIO_APP50
    #define UART3_RX D,9,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR1 = (AFIO->PCFR1 &~AFIO_PCFR1_USART3_REMAP ) | 3*AFIO_PCFR1_USART3_REMAP_0; \
    }while(0)
  #else
    #error USART3 remap must be defined as 0, 1, 2 or 3
  #endif
#endif

#if USART == 4
  #if USART_REMAP == 0
    #define UART4_TX C,10,1,GPIO_APP50
    #define UART4_RX C,11,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART4_RM) | 0*AFIO_PCFR2_USART4_RM; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART4_TX B,0,1,GPIO_APP50
    #define UART4_RX B,1,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART4_RM) | 1*AFIO_PCFR2_USART4_RM; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART4_TX E,0,1,GPIO_APP50
    #define UART4_RX E,1,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART4_RM) | 2*AFIO_PCFR2_USART4_RM; \
    }while(0)
  #else
    #error USART4 remap must be defined as 0, 1 or 2
  #endif
#endif

#if USART == 5
  #if USART_REMAP == 0
    #define UART5_TX C,12,1,GPIO_APP50
    #define UART5_RX D,2, 1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART5_RM) | 0*AFIO_PCFR2_USART5_RM; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART5_TX B,4,1,GPIO_APP50
    #define UART5_RX B,5,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART5_RM) | 1*AFIO_PCFR2_USART5_RM; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART5_TX E,8,1,GPIO_APP50
    #define UART5_RX E,9,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART5_RM) | 2*AFIO_PCFR2_USART5_RM; \
    }while(0)
  #else
    #error USART5 remap must be defined as 0, 1 or 2
  #endif
#endif
    
#if USART == 6
  #if USART_REMAP == 0
    #define UART6_TX C,0,1,GPIO_APP50
    #define UART6_RX C,1,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART6_RM) | 0*AFIO_PCFR2_USART6_RM; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART6_TX B,8,1,GPIO_APP50
    #define UART6_RX B,9,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART6_RM) | 1*AFIO_PCFR2_USART6_RM; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART6_TX E,10,1,GPIO_APP50
    #define UART6_RX E,11,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART6_RM) | 2*AFIO_PCFR2_USART6_RM; \
    }while(0)
  #else
    #error USART6 remap must be defined as 0, 1 or 2
  #endif
#endif
    
#if USART == 7
  #if USART_REMAP == 0
    #define UART7_TX C,2,1,GPIO_APP50
    #define UART7_RX C,3,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART7_RM) | 0*AFIO_PCFR2_USART7_RM; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART7_TX A,6,1,GPIO_APP50
    #define UART7_RX A,7,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART7_RM) | 1*AFIO_PCFR2_USART7_RM; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART7_TX E,12,1,GPIO_APP50
    #define UART7_RX E,13,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART7_RM) | 2*AFIO_PCFR2_USART7_RM; \
    }while(0)
  #else
    #error USART4 remap must be defined as 0, 1 or 2
  #endif
#endif
    
#if USART == 8
  #if USART_REMAP == 0
    #define UART8_TX C,4,1,GPIO_APP50
    #define UART8_RX C,5,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART8_RM) | 0*AFIO_PCFR2_USART8_RM; \
    }while(0)
  #elif USART_REMAP == 1
    #define UART8_TX A,14,1,GPIO_APP50
    #define UART8_RX A,15,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART8_RM) | 1*AFIO_PCFR2_USART8_RM; \
    }while(0)
  #elif USART_REMAP == 2
    #define UART8_TX E,14,1,GPIO_APP50
    #define UART8_RX E,15,1,GPIO_HIZ
    #define uart_do_remap() do{ \
      AFIO->PCFR2 = (AFIO->PCFR2 &~ 3*AFIO_PCFR2_USART8_RM) | 2*AFIO_PCFR2_USART8_RM; \
    }while(0)
  #else
    #error USART4 remap must be defined as 0, 1 or 2
  #endif
#endif

//UART1_init
void UARTn_func(USART, init)(uint16_t brr){
  GPIO_config( UART_PIN(USART, RX) );
  GPIO_config( UART_PIN(USART, TX) );
#if USART == 1
  RCC->APB2PCENR |= RCC_USART1EN;
#else
  RCC->APB1PCENR |= RCC_USARTnEN(USART);
#endif
#if USART_REMAP > 0
  RCC->APB2PCENR |= RCC_AFIOEN;
#endif
  uart_do_remap();
  UART(USART)->BRR = (brr);
  UART(USART)->CTLR1 = USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE | USART_CTLR1_RXNEIE;
  UART(USART)->CTLR2 = 0;
  UART(USART)->CTLR3 = 0;
  UART(USART)->GPR = 0;
  uartN_Dx(USART,rx).st=0; uartN_Dx(USART,rx).en=0; uartN_Dx(USART,tx).st=0; uartN_Dx(USART,tx).en=0;
  NVIC_EnableIRQ( UART_IRQ(USART) );
}

#define UART_speed(num, brr) do{UART(USART)->BRR = (brr);}while(0)
#define UART_parity_enable(num) do{UART(USART)->CTLR1 |= USART_CTLR1_PCE;}while(0)
#define UART_parity_disable(num) do{UART(USART)->CTLR1 &=~ USART_CTLR1_PCE;}while(0)
#define UART_parity_even(num) do{UART(USART)->CTLR1 &=~ USART_CTLR1_PS;}while(0)
#define UART_parity_odd(num) do{UART(USART)->CTLR1 |= USART_CTLR1_PS;}while(0)
#define UART_wordlen(num, len) do{if(len == 9)UART(USART)->CTLR1 |= USART_CTLR1_M; else UART(USART)->CTLR1 &=~ USART_CTLR1_M;}while(0)

///////////////////////////////////////////////////////////////////////////////////////////////
//             Interrupt
///////////////////////////////////////////////////////////////////////////////////////////////
#define _uart_IRQ(num) USART ## num ## _IRQHandler
#define uart_IRQ(num) _uart_IRQ(num)
//__attribute__((interrupt)) void USART2_IRQHandler(void){
__attribute__(( optimize("-Ofast") ))
__attribute__((interrupt)) void uart_IRQ(USART)(void){
  if( UART(USART)->STATR & USART_STATR_RXNE ){
    uint8_t temp = UART(USART)->DATAR;
    uart_buf_putc(&uartN_Dx(USART, rx), temp);
  }else if( UART(USART)->STATR & USART_STATR_TXE ){
    if(uart_buf_busy(&uartN_Dx(USART, tx)) != 0){
      UART(USART)->DATAR = uart_buf_getc(&uartN_Dx(USART, tx));
    }else{
      UART(USART)->CTLR1 &=~ USART_CTLR1_TXEIE;
    }
  }
}

#elif defined UART_DECLARATIONS

//UART1_write
void UARTn_func(UART_DECLARATIONS, write)(uint8_t *data, uint32_t len);
//UART1_puts
void UARTn_func(UART_DECLARATIONS, puts)(char *str);
//UART1_read
void UARTn_func(UART_DECLARATIONS, read)(uint8_t *data, uint32_t len);
//UART1_gets
void UARTn_func(UART_DECLARATIONS, gets)(char *str, uint32_t len);
//UART1_init
void UARTn_func(UART_DECLARATIONS, init)(uint16_t brr);

#else

  #error define either USART or UART_DECLARATIONS macro

#endif

#endif
