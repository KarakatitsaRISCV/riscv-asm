#include "ch32v30x.h"
#include "clock.h"
#include "strlib.h"
#include "pinmacro.h"

#include "FreeRTOS.h"
#include "task.h"

#ifndef NULL
  #define NULL ((void*)0)
#endif
uint32_t SystemCoreClock = 8000000;
void SystemInit(void){}

TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;

#define UART_TX A,9 ,1,GPIO_APP50
#define UART_RX A,10,1,GPIO_HIZ

#if 1
#define RLED	B,4 ,0,GPIO_PP50
#define GLED	A,11,1,GPIO_PP50
#else
#define RLED	B,5,1,GPIO_PP50
#define GLED	B,7,1,GPIO_PP50
#endif

const char uart_data[] = "Возможно, если бы у меня изначально был человеческий JTAG-программатор, было бы проще. Но моя нелюбовь покупать готовые отладочные устройства этому активно противится. Как бы то ни было, проблема решена, причем с избытком. Описываю я только свое решение, возможно с другими программаторами, контроллерами или софтом алгоритм будет отличаться.\r\n";

void task1_task(void *pvParameters){
  GPO_OFF(GLED); GPO_OFF(RLED);
  while(1){
    GPO_ON(GLED);
    vTaskDelay(200); //Задержка 200 тиков планировщика
    GPO_OFF(GLED);
    vTaskDelay(200);
    GPO_ON(RLED);
    vTaskDelay(200);
    GPO_OFF(RLED);
    vTaskDelay(200);
  }
}

void task2_task(void *pvParameters){
  uint16_t idx = 0;
  while(1){
    while( !(USART1->STATR & USART_STATR_TXE) ){} //Заметьте, здесь вообще нет задержек. Так делать нельзя, но это иллюстрация, что FreeRTOS умеет переключать задачи принудительно
    USART1->DATAR = uart_data[idx];
    idx++;
    if(idx > sizeof(uart_data))idx = 0;
  }
}

int main(){
 // clock_HS(1);
  systick_init();
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RLED); GPIO_config(GLED);
  GPO_OFF(RLED); GPO_OFF(GLED);
  
  GPIO_config( UART_TX );
  GPIO_config( UART_RX );
  RCC->APB2PCENR |= RCC_USART1EN;
  USART1->BRR = 8000000 / 9600;
  USART1->CTLR1 = USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE;
  
  delay_ticks(8000000);
  
  xTaskCreate((TaskFunction_t)task2_task,
              (const char*   )"task2",
              (uint16_t      )256, //stack size
              (void*         )NULL,
              (UBaseType_t   )5, //priority
              (TaskHandle_t* )&Task2Task_Handler);

  xTaskCreate((TaskFunction_t)task1_task,
              (const char*   )"task1",
              (uint16_t      )256, //stack size
              (void*         )NULL,
              (UBaseType_t   )5, //priority
              (TaskHandle_t* )&Task1Task_Handler);
  vTaskStartScheduler();
  while(1){}

}
