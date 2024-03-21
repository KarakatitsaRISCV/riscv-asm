#ifndef _USB_MIC_H_
#define _USB_MIC_H_
#include "hardware.h"
#include "adc.h"

#define F_SAMPLE 44000
#define ADC_SIZE (F_SAMPLE/1000)

static char mic_ep0_in(config_pack_t *req, void **data, uint16_t *size){return 0;}
static char mic_ep0_out(config_pack_t *req, uint16_t offset, uint16_t rx_size){return 0;}

volatile uint16_t adc_val = 0;
volatile int16_t adc_buffer[ADC_SIZE];
volatile uint16_t adc_vref = 0;

volatile uint8_t adc_idx = 0;
static void mic_in_callback(uint8_t epnum){
  usb_ep_write_double( epnum, (void*)adc_buffer, 2*adc_idx );
  adc_idx = 0;
};

#define OVERSAMPL_COUNT 4

#define TIM2_ARR_AVG (36000/1/ADC_SIZE/OVERSAMPL_COUNT)
#define TIM2_ARR_MAX (TIM2_ARR_AVG * 4)
#define TIM2_ARR_MIN (TIM2_ARR_AVG / 4)

static void mic_init(){
  usb_ep_init_double( ENDP_MIC | 0x80, USB_ENDP_ISO, ENDP_MIC_SIZE, mic_in_callback );
  GPIO_config(ADC_IN);
  
  //ADC2 - analog in
  ADC_init( ADC_2 );
  ADC_SAMPLING_TIME( ADC_2, marg3(ADC_IN), ADC_SAMPL_TIME_2 );
  ADC_SEQ_SET( ADC_2, 1, marg3(ADC_IN) );
  ADC_SEQ_CNT( ADC_2, 1 );
  ADC_Trigger( ADC_2, ADC_TRIG_TIM2_2 );
  ADC2->CR1 |= ADC_CR1_EOCIE;
  NVIC_EnableIRQ( ADC1_2_IRQn );
  
  //ADC1 - Vref
  ADC_init( ADC_1 );
  ADC1->CR2 |= ADC_CR2_TSVREFE;
  ADC_SAMPLING_TIME( ADC_1, ADC_CH_VREF, ADC_SAMPL_TIME_56 );
  ADC_SEQ_SET( ADC_1, 1, ADC_CH_VREF );
  ADC_SEQ_CNT( ADC_1, 1 );
  
  //Timer2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->PSC = 0;
  TIM2->ARR = TIM2_ARR_AVG;
  TIM2->CR1 |= TIM_CR1_ARPE;
  TIM2->CCMR1 = (0b011 << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
  TIM2->CCER = TIM_CCER_CC2E;
  TIM2->CR1 |= TIM_CR1_CEN;
}

#define OFILTER_PWR	8
#define OFILTER_MLT	4
uint16_t adc_outfilter(uint16_t curval){
  static uint32_t filter = (2048<<(OFILTER_PWR+OFILTER_MLT));
  filter += (curval << OFILTER_MLT) - (filter >> OFILTER_PWR);
  return (filter>>OFILTER_PWR);
}

#include "systick.h"
static void mic_poll(){
  static uint32_t ntime_ms = 0;
  
  if(systick_ms() < ntime_ms)return;
  ntime_ms = systick_ms() + 1000;

  if(ADC1->SR & ADC_SR_EOC){
    adc_vref = ADC1->DR << OFILTER_MLT;
    ADC1->CR2 |= ADC_CR2_ADON;
  }
}

#define MIDFILTER_PWR	10
uint16_t midfilter(uint16_t curval){
  static uint32_t filter = (2048<<MIDFILTER_PWR);
  filter += curval - (filter>>MIDFILTER_PWR);
  return (filter >> MIDFILTER_PWR);
}

#define INFILTER_PWR 0
uint16_t adc_infilter(uint16_t curval){
  static uint32_t filter = (2048<<INFILTER_PWR);
  filter += curval - (filter>>INFILTER_PWR);
  return (filter >> INFILTER_PWR);
}

volatile uint8_t adc_timeH = 0;
volatile uint16_t adc_timeL = 0;
void ADC1_2_IRQHandler(){
  static uint8_t oversampl_cnt = 0;
  static uint16_t curval = 0;
  
  //Read ADC with oversampling
  if(oversampl_cnt == 0)curval = 0;
  curval += ADC2->DR;
  oversampl_cnt++;
  if(oversampl_cnt < OVERSAMPL_COUNT)return;
  oversampl_cnt = 0;
  curval /= OVERSAMPL_COUNT;
  
  //calculate ADC (with low-pass filter) to Volts
  uint16_t adc = adc_outfilter(curval);
  adc_val = (uint32_t)120*adc/adc_vref; //Vref=1.2V ; fixed-point 2 dec.digts frac (*100)
  
  uint16_t cval = adc_infilter(curval);
  int32_t midval = midfilter(cval);
  midval = (((int32_t)cval)) - midval;
  
  midval <<= 3; //ADC resolution is 0-4095. midval=0-4095. Diff=-4095..+4095 (2**12); Output = -32768..+32767 (2**15) => Kampl=8
  
  
  //if buffer is not full - append it
  if(adc_idx < ADC_SIZE){
    adc_buffer[adc_idx] = midval;
    adc_idx++;
  }
  
  //adjustment timer frequency to SOF signal
  //adc_timeH - count of full conversions
  if(adc_timeH<255)adc_timeH++;
  adc_timeL = TIM2->CNT;

  ADC2->SR &=~ADC_SR_EOC;
}

void mic_sof(){
  if(adc_timeH > ADC_SIZE){
    if(TIM2->ARR < TIM2_ARR_MAX) TIM2->ARR++;
  }else if(adc_timeH < ADC_SIZE){
    if(TIM2->ARR > TIM2_ARR_MIN) TIM2->ARR--;
  }else{
    if(adc_timeL > (TIM2->ARR/2)){
      TIM2->ARR++;
    }else if(adc_timeL < (TIM2->ARR*3/4)){
      TIM2->ARR--;
    }
  }
  TIM2->CNT = 0;
  TIM2->CR1 |= TIM_CR1_URS;
  adc_timeH = 0;
}

#endif
