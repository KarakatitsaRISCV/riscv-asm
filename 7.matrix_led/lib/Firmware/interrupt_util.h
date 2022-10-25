#ifndef _UTIL_H_
#define _UTIL_H_

#include "riscv_encoding.h"
#include "n200_func.h"

//   (из файла GD32VF103_standard_peripheral/Source/gd32vf103_eclic)
//разрешить прерывания глобально
#define eclic_global_interrupt_enable() set_csr(mstatus, MSTATUS_MIE)
//запретить прерывания глобально
#define eclic_global_interrupt_disable() clear_csr(mstatus, MSTATUS_MIE)

__attribute__((naked)) int main();

//прописываем для всех прерываний прототипы, отмеченные именно как прерывания
__attribute__((interrupt)) void eclic_msip_handler(void);
__attribute__((interrupt)) void eclic_mtip_handler(void);
__attribute__((interrupt)) void eclic_bwei_handler(void);
__attribute__((interrupt)) void eclic_pmovi_handler(void);
__attribute__((interrupt)) void WWDGT_IRQHandler(void);
__attribute__((interrupt)) void LVD_IRQHandler(void);
__attribute__((interrupt)) void TAMPER_IRQHandler(void);
__attribute__((interrupt)) void RTC_IRQHandler(void);
__attribute__((interrupt)) void FMC_IRQHandler(void);
__attribute__((interrupt)) void RCU_IRQHandler(void);
__attribute__((interrupt)) void EXTI0_IRQHandler(void);
__attribute__((interrupt)) void EXTI1_IRQHandler(void);
__attribute__((interrupt)) void EXTI2_IRQHandler(void);
__attribute__((interrupt)) void EXTI3_IRQHandler(void);
__attribute__((interrupt)) void EXTI4_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel0_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel1_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel2_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel3_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel4_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel5_IRQHandler(void);
__attribute__((interrupt)) void DMA0_Channel6_IRQHandler(void);
__attribute__((interrupt)) void ADC0_1_IRQHandler(void);
__attribute__((interrupt)) void CAN0_TX_IRQHandler(void);
__attribute__((interrupt)) void CAN0_RX0_IRQHandler(void);
__attribute__((interrupt)) void CAN0_RX1_IRQHandler(void);
__attribute__((interrupt)) void CAN0_EWMC_IRQHandler(void);
__attribute__((interrupt)) void EXTI5_9_IRQHandler(void);
__attribute__((interrupt)) void TIMER0_BRK_IRQHandler(void);
__attribute__((interrupt)) void TIMER0_UP_IRQHandler(void);
__attribute__((interrupt)) void TIMER0_TRG_CMT_IRQHandler(void);
__attribute__((interrupt)) void TIMER0_Channel_IRQHandler(void);
__attribute__((interrupt)) void TIMER1_IRQHandler(void);
__attribute__((interrupt)) void TIMER2_IRQHandler(void);
__attribute__((interrupt)) void TIMER3_IRQHandler(void);
__attribute__((interrupt)) void I2C0_EV_IRQHandler(void);
__attribute__((interrupt)) void I2C0_ER_IRQHandler(void);
__attribute__((interrupt)) void I2C1_EV_IRQHandler(void);
__attribute__((interrupt)) void I2C1_ER_IRQHandler(void);
__attribute__((interrupt)) void SPI0_IRQHandler(void);
__attribute__((interrupt)) void SPI1_IRQHandler(void);
__attribute__((interrupt)) void USART0_IRQHandler(void);
__attribute__((interrupt)) void USART1_IRQHandler(void);
__attribute__((interrupt)) void USART2_IRQHandler(void);
__attribute__((interrupt)) void EXTI10_15_IRQHandler(void);
__attribute__((interrupt)) void RTC_Alarm_IRQHandler(void);
__attribute__((interrupt)) void USBFS_WKUP_IRQHandler(void);
__attribute__((interrupt)) void EXMC_IRQHandler(void);
__attribute__((interrupt)) void TIMER4_IRQHandler(void);
__attribute__((interrupt)) void SPI2_IRQHandler(void);
__attribute__((interrupt)) void UART3_IRQHandler(void);
__attribute__((interrupt)) void UART4_IRQHandler(void);
__attribute__((interrupt)) void TIMER5_IRQHandler(void);
__attribute__((interrupt)) void TIMER6_IRQHandler(void);
__attribute__((interrupt)) void DMA1_Channel0_IRQHandler(void);
__attribute__((interrupt)) void DMA1_Channel1_IRQHandler(void);
__attribute__((interrupt)) void DMA1_Channel2_IRQHandler(void);
__attribute__((interrupt)) void DMA1_Channel3_IRQHandler(void);
__attribute__((interrupt)) void DMA1_Channel4_IRQHandler(void);
__attribute__((interrupt)) void CAN1_TX_IRQHandler(void);
__attribute__((interrupt)) void CAN1_RX0_IRQHandler(void);
__attribute__((interrupt)) void CAN1_RX1_IRQHandler(void);
__attribute__((interrupt)) void CAN1_EWMC_IRQHandler(void);
__attribute__((interrupt)) void USBFS_IRQHandler(void);



__attribute__((interrupt)) void IRQHandler(void);
__attribute__((interrupt)) void NMIHandler(void);
__attribute__((interrupt)) void TrapHandler(void);
__attribute__((interrupt)) void UnhandledInterruptHandler(void);


#endif
