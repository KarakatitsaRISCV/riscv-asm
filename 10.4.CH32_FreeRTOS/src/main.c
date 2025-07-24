#include "ch32v30x.h"
#include "hardware.h"
#include "clock.h"
#define USART 1
#define UART_SIZE_PWR 10//5
#include "uart.h"
#include "strlib.h"

#include "FreeRTOS.h"
#include "task.h"

#ifndef NULL
  #define NULL ((void*)0)
#endif
uint32_t SystemCoreClock = 144000000;
void SystemInit(void){}

TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;

void task1_task(void *pvParameters){
  while(1){
    UART_puts(USART, "Task1\r\n");
    GPO_T(GLED);
    vTaskDelay(250); //Задержка 250 тиков планировщика
  }
}

void task2_task(void *pvParameters){
  while(1){
    UART_puts(USART, "Task2\r\n");
    GPO_T(RLED);
    delay_ticks(144000000); // На самом деле так делать нельзя, но это иллюстрирует как FreeRTOS переключает задачи без обязательного вызова функций ОС
  }
}

int main(){
  clock_HS(1);
  systick_init();
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  GPIO_config(RBTN); GPIO_config(SBTN);
  GPIO_config(RLED); GPIO_config(GLED); GPIO_config(YLED);
  GPO_OFF(RLED); GPO_OFF(GLED); GPO_OFF(YLED);
  
  UART_init(USART, 144000000 / 2 / 115200);
  UART_puts(USART, "\r\n---"__TIME__ "---\r\n");
  
  delay_ticks(144000000);
  
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
