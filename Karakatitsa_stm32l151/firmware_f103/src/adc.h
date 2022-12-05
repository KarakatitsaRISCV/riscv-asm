#ifndef _ADC_H_
#define _ADC_H_

#include "pinmacro.h"

#define ADC_1	1
#define ADC_2	2

//only on ADC1 master avaible
#define ADC_CH_VREF 17 //внутренний ИОН ~1.2 В
#define ADC_CH_TEMP 16 //внутренний датчик температуры, (Tsample >= 17.1 us)

//conv_time = 12.5
#define ADC_SAMPL_TIME_2	0 //1.5		-> 14
#define ADC_SAMPL_TIME_8	1 //7.5		-> 20
#define ADC_SAMPL_TIME_14	2 //13.5	-> 26
#define ADC_SAMPL_TIME_29	3 //28.5	-> 41
#define ADC_SAMPL_TIME_42	4 //41.5	-> 54
#define ADC_SAMPL_TIME_56	5 //55.5	-> 68
#define ADC_SAMPL_TIME_72	6 //71.5	-> 84
#define ADC_SAMPL_TIME_240	7 //239.5	-> 252

#define ADC_SPEED_1 0
#define ADC_SPEED_2 1
#define ADC_SPEED_4 2

#define _ADC(x)	ADC##x
#define ADC(x) _ADC(x)


#define ADCSQR_1  3
#define ADCSQR_2  3
#define ADCSQR_3  3
#define ADCSQR_4  3
#define ADCSQR_5  3
#define ADCSQR_6  3

#define ADCSQR_7  2
#define ADCSQR_8  2
#define ADCSQR_9  2
#define ADCSQR_10 2
#define ADCSQR_11 2
#define ADCSQR_12 2

#define ADCSQR_13 1
#define ADCSQR_14 1
#define ADCSQR_15 1
#define ADCSQR_16 1

#define __ADC_SQR(adc, sqr) ADC##adc ->SQR##sqr
#define _ADC_SQR(adc, sqr) __ADC_SQR(adc, sqr)
#define ADC_SQR(adc, num) _ADC_SQR(adc, ADCSQR_##num)

#define __ADC_SQR_SQ(rnum, num) ADC_SQR ## rnum ## _SQ ## num
#define _ADC_SQR_SQ(rnum, num) __ADC_SQR_SQ(rnum, num)
#define ADC_SQR_SQ(num) _ADC_SQR_SQ(ADCSQR_##num, num)

#define ADC_SEQ_SET(adc, elem, val) do{\
  PM_BITMASK( ADC_SQR(adc, elem), ADC_SQR_SQ(elem), val );\
  /*while( (ADC(adc)->SR & ADC_SR_RCNR) ){}*/ \
}while(0)

#define _ADC_SEQ_CNT(adc, cnt) PM_BITMASK( ADC##adc ->SQR1, ADC_SQR1_L, (cnt) )
#define ADC_SEQ_CNT(adc, cnt) _ADC_SEQ_CNT(adc, (cnt)-1)

//SMPR
#define ADCSMPR_0  2
#define ADCSMPR_1  2
#define ADCSMPR_2  2
#define ADCSMPR_3  2
#define ADCSMPR_4  2
#define ADCSMPR_5  2
#define ADCSMPR_6  2
#define ADCSMPR_7  2
#define ADCSMPR_8  2
#define ADCSMPR_9  2

#define ADCSMPR_10 1
#define ADCSMPR_11 1
#define ADCSMPR_12 1
#define ADCSMPR_13 1
#define ADCSMPR_14 1
#define ADCSMPR_15 1
#define ADCSMPR_16 1
#define ADCSMPR_17 1

#define __ADC_SMPR(adc, smp) ADC##adc ->SMPR##smp
#define _ADC_SMPR(adc, smp) __ADC_SMPR(adc, smp)
#define ADC_SMPR(adc, num) _ADC_SMPR(adc, ADCSMPR_##num)

#define __ADC_SMPR_SMP(smp, num) ADC_SMPR ## smp ##_SMP ## num
#define _ADC_SMPR_SMP(smp, num) __ADC_SMPR_SMP(smp, num)
#define ADC_SMPR_SMP(num) _ADC_SMPR_SMP(ADCSMPR_##num, num)

#define ADC_SAMPLING_TIME(adc, chan, time) PM_BITMASK( ADC_SMPR(adc, chan), ADC_SMPR_SMP(chan), time )

#define ADC_ENR(num) RCC_APB2ENR_ADC ## num ## EN

//RCC_CFGR
#define ADCPRE2 (0 << RCC_CFGR_ADCPRE_Pos)
#define ADCPRE4 (1 << RCC_CFGR_ADCPRE_Pos)
#define ADCPRE6 (2 << RCC_CFGR_ADCPRE_Pos)
#define ADCPRE8 (3 << RCC_CFGR_ADCPRE_Pos)
#define _ADCPRE(x) ADCPRE##x
#define ADCPRE(x) _ADCPRE(x)

#define ADC_TRIG_TIM1_1	(0<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_TIM1_2	(1<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_TIM1_3	(2<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_TIM2_2	(3<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_TIM3TR	(4<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_TIM4_4	(5<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_EXTI11_TIM8TR	(6<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_SW		(7<<ADC_CR2_EXTSEL_Pos) | ADC_CR2_EXTTRIG
#define ADC_TRIG_DISABLE (0)

#define ADC_Trigger( num, trigger ) do{ \
    ADC(num)->CR2 = (ADC(num)->CR2 &~(ADC_CR2_EXTSEL_Msk | ADC_CR2_EXTTRIG)) | trigger; \
  }while(0)
  

//APB2(fast)->ADCPRE->14MHz
#define _ADC_init( num ) do{ \
  RCC->APB2ENR |= ADC_ENR(num); \
  ADC(num)->CR2 |= ADC_CR2_ADON; \
  PM_BITMASK( RCC->CFGR, RCC_CFGR_ADCPRE, ADCPRE6 ); /* 72/6 = 12MHz */\
  ADC_CAL( num ); \
}while(0)
#define ADC_init(num) _ADC_init( num )


#define ADC_CAL(num) do{ \
    for(uint32_t i=0; i<20; i++)asm volatile("nop"); \
    ADC(num)->CR2 |= ADC_CR2_CAL; \
    while(ADC(num)->CR2 & ADC_CR2_CAL){} \
  }while(0)
  
/*
static inline uint16_t adc1_read(uint8_t chan){
  ADC_SEQ_SET( ADC_1, 1, chan );
  ADC1->CR2 |= ADC_CR2_ADON;
  while(!(ADC1->SR & ADC_SR_EOC)){}
  return ADC1->DR;
}

static inline uint16_t adc2_read(uint8_t chan){
  ADC_SEQ_SET( ADC_2, 1, chan );
  ADC2->CR2 |= ADC_CR2_ADON;
  while(!(ADC2->SR & ADC_SR_EOC)){}
  return ADC2->DR;
}*/

#endif
