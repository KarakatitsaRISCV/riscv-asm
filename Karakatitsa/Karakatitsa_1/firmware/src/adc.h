#ifndef _ADC_H_
#define _ADC_H_
#include "pinmacro.h"

#define ADC_1 1

#define VREFINT_CAL (*((uint16_t*)0x1FF800F8)) /*значение VREF при AVCC=3V*/
#define ADC_CH_VREF 17 /*внутренний ИОН ~1.2 В*/
#define ADC_CH_TEMP 16 /*внутренний датчик температуры*/

#define ADC_SAMPL_TIME_4   0
#define ADC_SAMPL_TIME_9   1
#define ADC_SAMPL_TIME_16  2
#define ADC_SAMPL_TIME_24  3
#define ADC_SAMPL_TIME_48  4
#define ADC_SAMPL_TIME_96  5
#define ADC_SAMPL_TIME_192 6
#define ADC_SAMPL_TIME_384 7

#define ADC_SPEED_1 0
#define ADC_SPEED_2 1
#define ADC_SPEED_4 2

#define _ADCx(x) ADC##x
#define ADCx(x)  _ADCx(x)


#define ADCSQR_1  5
#define ADCSQR_2  5
#define ADCSQR_3  5
#define ADCSQR_4  5
#define ADCSQR_5  5
#define ADCSQR_6  5

#define ADCSQR_7  4
#define ADCSQR_8  4
#define ADCSQR_9  4
#define ADCSQR_10 4
#define ADCSQR_11 4
#define ADCSQR_12 4

#define ADCSQR_13 3
#define ADCSQR_14 3
#define ADCSQR_15 3
#define ADCSQR_16 3
#define ADCSQR_17 3
#define ADCSQR_18 3

#define ADCSQR_19 2
#define ADCSQR_20 2
#define ADCSQR_21 2
#define ADCSQR_22 2
#define ADCSQR_23 2
#define ADCSQR_24 2

#define ADCSQR_25 1
#define ADCSQR_26 1
#define ADCSQR_27 1
#define ADCSQR_28 1

#define __ADC_SQR(adc, sqr) ADC##adc ->SQR##sqr
#define _ADC_SQR(adc, sqr) __ADC_SQR(adc, sqr)
#define ADC_SQR(adc, num) _ADC_SQR(adc, ADCSQR_##num)

#define __ADC_SQR_SQ(rnum, num) ADC_SQR ## rnum ## _SQ ## num
#define _ADC_SQR_SQ(rnum, num) __ADC_SQR_SQ(rnum, num)
#define ADC_SQR_SQ(num) _ADC_SQR_SQ(ADCSQR_##num, num)

#define ADC_SEQ_SET(adc, elem, val) do{\
  PM_BITMASK( ADC_SQR(adc, elem), ADC_SQR_SQ(elem), val );\
  while( (ADC1->SR & ADC_SR_RCNR) ){} \
}while(0)

#define _ADC_SEQ_CNT(adc, cnt) PM_BITMASK( ADC##adc ->SQR1, ADC_SQR1_L, (cnt) )
#define ADC_SEQ_CNT(adc, cnt) _ADC_SEQ_CNT(adc, (cnt)-1)

//SMPR
#define ADCSMPR_0  3
#define ADCSMPR_1  3
#define ADCSMPR_2  3
#define ADCSMPR_3  3
#define ADCSMPR_4  3
#define ADCSMPR_5  3
#define ADCSMPR_6  3
#define ADCSMPR_7  3
#define ADCSMPR_8  3
#define ADCSMPR_9  3

#define ADCSMPR_10 2
#define ADCSMPR_11 2
#define ADCSMPR_12 2
#define ADCSMPR_13 2
#define ADCSMPR_14 2
#define ADCSMPR_15 2
#define ADCSMPR_16 2
#define ADCSMPR_17 2
#define ADCSMPR_18 2
#define ADCSMPR_19 2

#define ADCSMPR_20 1
#define ADCSMPR_21 1
#define ADCSMPR_22 1
#define ADCSMPR_23 1
#define ADCSMPR_24 1
#define ADCSMPR_25 1
#define ADCSMPR_26 1
#define ADCSMPR_27 1
#define ADCSMPR_28 1
#define ADCSMPR_29 1

#define __ADC_SMPR(adc, smp) ADC##adc ->SMPR##smp
#define _ADC_SMPR(adc, smp) __ADC_SMPR(adc, smp)
#define ADC_SMPR(adc, num) _ADC_SMPR(adc, ADCSMPR_##num)

#define __ADC_SMPR_SMP(smp, num) ADC_SMPR ## smp ##_SMP ## num
#define _ADC_SMPR_SMP(smp, num) __ADC_SMPR_SMP(smp, num)
#define ADC_SMPR_SMP(num) _ADC_SMPR_SMP(ADCSMPR_##num, num)

#define ADC_SAMPLING_TIME(adc, chan, time) PM_BITMASK( ADC_SMPR(adc, chan), ADC_SMPR_SMP(chan), time );
//ADC1->SMPR3
//ADC_SMP3_SMP0
/*void test(){
  ADC_SQR(1,1) = ADC_SQR_SQ(1);
}
//*/

#define ADC_TRIG_TIM9_CC2	0
#define ADC_TRIG_TIM9_TRIG	1
#define ADC_TRIG_TIM2_3		2
#define ADC_TRIG_TIM2_2		3
#define ADC_TRIG_TIM3_TRIG	4
#define ADC_TRIG_TIM4_4		5
#define ADC_TRIG_TIM2_TRIG	6
#define ADC_TRIG_TIM3_1		7
#define ADC_TRIG_TIM3_3		8
#define ADC_TRIG_TIM4_TRIG	9
#define ADC_TRIG_TIM6_TRIG	10
#define ADC_TRIG_EXTI11		15

#define ADC_Trigger( num, trigger ) do{ \
  uint32_t temp = ADCx(num)->CR2; \
  PM_BITMASK( temp, ADC_CR2_EXTSEL, trigger ); \
  PM_BITMASK(temp, ADC_CR2_EXTEN, 0b01 ); \
  ADCx(num)->CR2 = temp; \
  }while(0)

#endif
