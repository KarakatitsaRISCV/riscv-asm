#ifndef __TIMER_H__
#define __TIMER_H__

#if 1==0
  #define IO	A,11,0,GPIO_APP50
  #define TIM 1,4,TIMO_PWM_NINV

  GPIO_config(IO);
  timer_init(TIM, 14400-1, 10000-1);
  timer_chval(TIM) = 1000;
  timer_chcfg(TIM);
  timer_chpol(TIM, IO);
  timer_enable(TIM);
  
  TIMO_ON(TIM);
  TIMO_OFF(TIM);
  TIMO_DEF(TIM);
#endif
  
#define TIMO_NONE		0
#define TIMO_SET_ACT	1
#define TIMO_SET_INACT	2
#define TIMO_INV		3
#define TIMO_FSET_INACT	4 //чем отличается от 2? Мгновенная реакция что ли?
#define TIMO_FSET_ACT	5 //чем отличается от 1?
#define TIMO_PWM_NINV	6
#define TIMO_PWM_INV	7
#define TIMO_MASK		0b111
#define TIMMODE(tnum, chan, mode, ...) ((mode) & TIMO_MASK)
  
#define TIMO_POS	(1<<6)
#define TIMO_NEG	(1<<7)
#define TIM_REMAP_pos	4
#define TIMO_REMAP0	(0b00<<TIM_REMAP_pos)
#define TIMO_REMAP1	(0b01<<TIM_REMAP_pos)
#define TIMO_REMAP2	(0b10<<TIM_REMAP_pos)
#define TIMO_REMAP3	(0b11<<TIM_REMAP_pos)
#define TIMO_REMAP_MASK (0b11<<TIM_REMAP_pos)
  
#define _TIMx(x,...)	TIM ## x
#define TIMx(x)		_TIMx(x)
#define _APBTIM(x)	APBTIM_ ## x
#define APBTIM(x)	_APBTIM(x)
#define ___CHCTRLx(chan)	CHCTLR ## chan
#define __CHCTRLx(chan) ___CHCTRLx(chan)
#define _CHCTRLx(chan) __CHCTRLx(TIMCHAN(chan))
#define CHCTRLx(tnum,y,...) _CHCTRLx(y)
#define _TIM_OCxM(chan)	TIM_OC ## chan ## M
#define TIM_OCxM(tnum,y,...) _TIM_OCxM(y)
#define _TIMx_CH1CVR(chan) CH ## chan ## CVR
#define TIMx_CH1CVR(tnum, chan,...) _TIMx_CH1CVR(chan)
#define _TIMx_CCE(chan)	TIM_CC ## chan ## E
#define TIMx_CCE(tnum, chan, ...) _TIMx_CCE(chan)
#define _TIMx_CCNE(chan)	TIM_CC ## chan ## NE
#define TIMx_CCNE(tnum, chan, ...) _TIMx_CCNE(chan)
#define _TIMx_CCP(chan)	TIM_CC ## chan ## P
#define TIMx_CCP(tnum, chan, ...) _TIMx_CCP(chan)
#define _TIMx_CCcS(chan)	TIM_CC ## chan ## S
#define TIMx_CCcS(tnum, chan, ...) _TIMx_CCcS(chan)
#define _TIM_DMA(tnum, chan, ...) TIM_DMA_ ## tnum ## _ ## chan
#define TIM_REMAP(tnum, chan, val, ...) TIM ## tnum ## _REMAP( ((val) & TIMO_REMAP_MASK) )
  
#define TIMER_CLOCK(num, enflag) do{ if(enflag) marg1(APBTIM(num))(|=); else marg1(APBTIM(num))(&=~);}while(0)
  
#define timer_init(tim, psc, top) do{\
    TIMER_CLOCK(_marg1(tim), 1); \
    _TIMx(tim)->PSC = (psc); \
    _TIMx(tim)->ATRLR = (top); \
  }while(0)
  
#define timer_chcfg(tim) do{ \
    PM_BITMASK(_TIMx(tim)->CHCTRLx(tim), TIM_OCxM(tim), TIMMODE(tim)); \
    if((TIMMODE(tim) >= TIMO_SET_ACT) && (TIMMODE(tim) <= TIMO_PWM_INV)){ \
      PM_BITMASK(_TIMx(tim)->CHCTRLx(tim), TIMx_CCcS(tim), 0b00); \
      if((_marg1(tim)==1)||(_marg1(tim)==8)||(_marg1(tim)==9)||(_marg1(tim)==10)){ \
        _TIMx(tim)->BDTR |= TIM_MOE; \
        if((_marg3(tim)) & TIMO_POS)_TIMx(tim)->CCER |= TIMx_CCE(tim); \
        if(((_marg3(tim)) & TIMO_NEG)&&(_marg2(tim)!=4)) _TIMx(tim)->CCER |= TIMx_CCNE(tim); \
      }else{ \
        _TIMx(tim)->CCER |= TIMx_CCE(tim); \
      }\
      TIM_REMAP(tim); \
    }\
  }while(0)
  
#define timer_chpol(tim, gpio) do{ \
    if(_marg3(gpio)==1)_TIMx(tim)->CCER &=~ TIMx_CCP(tim); \
      else _TIMx(tim)->CCER |= TIMx_CCP(tim); \
    }while(0)
  
#define timer_enable(tim) do{_TIMx(tim)->CTLR1 |= TIM_CEN;}while(0)
#define timer_cnt(tim) _TIMx(tim)->CNT
#define timer_chval(tim) _TIMx(tim)->TIMx_CH1CVR(tim)
#define timer_dma(tim)	_TIM_DMA(tim)
  
#define TIMO_ON(tim) do{PM_BITMASK(_TIMx(tim)->CHCTRLx(tim), TIM_OCxM(tim), TIMO_FSET_ACT);}while(0)
#define TIMO_OFF(tim) do{PM_BITMASK(_TIMx(tim)->CHCTRLx(tim), TIM_OCxM(tim), TIMO_FSET_INACT);}while(0)
#define TIMO_DEF(tim) do{PM_BITMASK(_TIMx(tim)->CHCTRLx(tim), TIM_OCxM(tim), TIMMODE(tim));}while(0)
  
//спасибо ленивм китайцам из WCH, что не прописали эти дефайны сами!
#define APBTIM_1(op)	RCC->APB2PCENR op (1<<11)
#define APBTIM_2(op)	RCC->APB1PCENR op (1<<0)
#define APBTIM_3(op)	RCC->APB1PCENR op (1<<1)
#define APBTIM_4(op)	RCC->APB1PCENR op (1<<2)
#define APBTIM_5(op)	RCC->APB1PCENR op (1<<3)
#define APBTIM_6(op)	RCC->APB1PCENR op (1<<4)
#define APBTIM_7(op)	RCC->APB1PCENR op (1<<5)
#define APBTIM_8(op)	RCC->APB2PCENR op (1<<13)
#define APBTIM_9(op)	RCC->APB2PCENR op (1<<19)
#define APBTIM_10(op)	RCC->APB2PCENR op (1<<20)
    
#define TIM_CC4NE	(1<<14)


#define TIMCHAN_1	1
#define TIMCHAN_2	1
#define TIMCHAN_3	2
#define TIMCHAN_4	2
#define _TIMCHAN(x) TIMCHAN_ ## x
#define TIMCHAN(x) _TIMCHAN(x)
  
#define TIM_DMA_1_1		1,2,do{TIM1->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM1->DMAINTENR &=~ TIM_CC1DE}while(0)
#define TIM_DMA_1_2		1,3,do{TIM1->DMAINTENR |= TIM_CC2DE;}while(0),do{TIM1->DMAINTENR &=~ TIM_CC2DE}while(0)
#define TIM_DMA_1_4		1,4,do{TIM1->DMAINTENR |= TIM_CC4DE;}while(0),do{TIM1->DMAINTENR &=~ TIM_CC4DE}while(0)
#define TIM_DMA_1_TRIG	1,4,do{TIM1->DMAINTENR |= TIM_TDE;}while(0),do{TIM1->DMAINTENR &=~ TIM_TDE}while(0)
#define TIM_DMA_1_COM	1,4,do{TIM1->DMAINTENR |= TIM_COMDE;}while(0),do{TIM1->DMAINTENR &=~ TIM_COMDE}while(0)
#define TIM_DMA_1_UP	1,5,do{TIM1->DMAINTENR |= TIM_UDE;}while(0),do{TIM1->DMAINTENR &=~ TIM_UDE}while(0)
#define TIM_DMA_1_3		1,6,do{TIM1->DMAINTENR |= TIM_CC3DE;}while(0),do{TIM1->DMAINTENR &=~ TIM_CC3DE}while(0)
  
#define TIM_DMA_2_3		1,1,do{TIM2->DMAINTENR |= TIM_CC3DE;}while(0),do{TIM2->DMAINTENR &=~ TIM_CC3DE}while(0)
#define TIM_DMA_2_UP	1,2,do{TIM2->DMAINTENR |= TIM_UDE;}while(0),do{TIM2->DMAINTENR &=~ TIM_UDE}while(0)
#define TIM_DMA_2_1		1,5,do{TIM2->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM2->DMAINTENR &=~ TIM_CC1DE}while(0)
#define TIM_DMA_2_2		1,7,do{TIM2->DMAINTENR |= TIM_CC2DE;}while(0),do{TIM2->DMAINTENR &=~ TIM_CC2DE}while(0)
#define TIM_DMA_2_4		1,7,do{TIM2->DMAINTENR |= TIM_CC4DE;}while(0),do{TIM2->DMAINTENR &=~ TIM_CC4DE}while(0)
  
#define TIM_DMA_3_3		1,2,do{TIM3->DMAINTENR |= TIM_CC3DE;}while(0),do{TIM3->DMAINTENR &=~ TIM_CC3DE}while(0)
#define TIM_DMA_3_4		1,3,do{TIM3->DMAINTENR |= TIM_CC4DE;}while(0),do{TIM3->DMAINTENR &=~ TIM_CC4DE}while(0)
#define TIM_DMA_3_UP	1,3,do{TIM3->DMAINTENR |= TIM_UDE;}while(0),do{TIM3->DMAINTENR &=~ TIM_UDE}while(0)
#define TIM_DMA_3_1		1,6,do{TIM3->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM3->DMAINTENR &=~ TIM_CC1DE}while(0)
#define TIM_DMA_3_TRIG	1,6,do{TIM3->DMAINTENR |= TIM_TDE;}while(0),do{TIM3->DMAINTENR &=~ TIM_TDE}while(0)
  
#define TIM_DMA_4_1		1,1,do{TIM4->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM4->DMAINTENR &=~ TIM_CC1DE}while(0)
#define TIM_DMA_4_2		1,4,do{TIM4->DMAINTENR |= TIM_CC2DE;}while(0),do{TIM4->DMAINTENR &=~ TIM_CC2DE}while(0)
#define TIM_DMA_4_3		1,5,do{TIM4->DMAINTENR |= TIM_CC3DE;}while(0),do{TIM4->DMAINTENR &=~ TIM_CC3DE}while(0)
#define TIM_DMA_4_UP	1,7,do{TIM4->DMAINTENR |= TIM_UDE;}while(0),do{TIM4->DMAINTENR &=~ TIM_UDE}while(0)
  
#define TIM_DMA_5_4		2,1,do{TIM5->DMAINTENR |= TIM_CC4DE;}while(0),do{TIM5->DMAINTENR &=~ TIM_CC4DE}while(0)
#define TIM_DMA_5_TRIG	2,1,do{TIM5->DMAINTENR |= TIM_TDE;}while(0),do{TIM5->DMAINTENR &=~ TIM_TDE}while(0)
#define TIM_DMA_5_3		2,2,do{TIM5->DMAINTENR |= TIM_CC3DE;}while(0),do{TIM5->DMAINTENR &=~ TIM_CC3DE}while(0)
#define TIM_DMA_5_UP	2,2,do{TIM5->DMAINTENR |= TIM_UDE;}while(0),do{TIM5->DMAINTENR &=~ TIM_UDE}while(0)
#define TIM_DMA_5_2		2,4,do{TIM5->DMAINTENR |= TIM_CC2DE;}while(0),do{TIM5->DMAINTENR &=~ TIM_CC2DE}while(0)
#define TIM_DMA_5_1		2,5,do{TIM5->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM5->DMAINTENR &=~ TIM_CC1DE}while(0)
  
#define TIM_DMA_6_UP	2,3,do{TIM6->DMAINTENR |= TIM_UDE;}while(0),do{TIM6->DMAINTENR &=~ TIM_UDE}while(0)
  
#define TIM_DMA_7_UP	2,4,do{TIM7->DMAINTENR |= TIM_UDE;}while(0),do{TIM7->DMAINTENR &=~ TIM_UDE}while(0)
  
#define TIM_DMA_8_3		2,1,do{TIM8->DMAINTENR |= TIM_CC3DE;}while(0),do{TIM8->DMAINTENR &=~ TIM_CC3DE}while(0)
#define TIM_DMA_8_UP	2,1,do{TIM8->DMAINTENR |= TIM_UDE;}while(0),do{TIM8->DMAINTENR &=~ TIM_UDE}while(0)
#define TIM_DMA_8_4		2,2,do{TIM8->DMAINTENR |= TIM_CC4DE;}while(0),do{TIM8->DMAINTENR &=~ TIM_CC4DE}while(0)
#define TIM_DMA_8_TRIG	2,2,do{TIM8->DMAINTENR |= TIM_TDE;}while(0),do{TIM8->DMAINTENR &=~ TIM_TDE}while(0)
#define TIM_DMA_8_COM	2,2,do{TIM18->DMAINTENR |= TIM_COMDE;}while(0),do{TIM8->DMAINTENR &=~ TIM_COMDE}while(0)
#define TIM_DMA_8_1		2,3,do{TIM8->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM8->DMAINTENR &=~ TIM_CC1DE}while(0)
#define TIM_DMA_8_2		2,5,do{TIM8->DMAINTENR |= TIM_CC2DE;}while(0),do{TIM8->DMAINTENR &=~ TIM_CC2DE}while(0)
  
#define TIM_DMA_9_UP	2,6,do{TIM9->DMAINTENR |= TIM_UDE;}while(0),do{TIM9->DMAINTENR &=~ TIM_UDE}while(0)
#define TIM_DMA_9_1		2,7,do{TIM9->DMAINTENR |= TIM_CC1DE;}while(0),do{TIM9->DMAINTENR &=~ TIM_CC1DE}while(0)
  
#define TIM_DMA_10_4	2,6,do{TIM10->DMAINTENR |= TIM_CC4DE;}while(0),do{TIM10->DMAINTENR &=~ TIM_CC4DE}while(0)
#define TIM_DMA_10_TRIG	2,7,do{TIM10->DMAINTENR |= TIM_TDE;}while(0),do{TIM10->DMAINTENR &=~ TIM_TDE}while(0)
#define TIM_DMA_10_COM	2,7,do{TIM10->DMAINTENR |= TIM_COMDE;}while(0),do{TIM10->DMAINTENR &=~ TIM_COMDE}while(0)
    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TIM1_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR1, AFIO_PCFR1_TIM1_REMAP, (val>>TIM_REMAP_pos)); }while(0)
#define TIM2_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR1, AFIO_PCFR1_TIM2_REMAP, (val>>TIM_REMAP_pos)); }while(0)
#define TIM3_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR1, AFIO_PCFR1_TIM3_REMAP, (val>>TIM_REMAP_pos)); }while(0)
#define TIM4_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR1, AFIO_PCFR1_TIM4_REMAP, (val>>TIM_REMAP_pos)); }while(0)
#define TIM5_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR1, AFIO_PCFR1_TIM5CH4_IREMAP, (val>>TIM_REMAP_pos)); }while(0)
  #define TIM6_REMAP(val) 	do{  }while(0)
  #define TIM7_REMAP(val) 	do{  }while(0)
#define TIM8_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR2, (1<<2), (val>>TIM_REMAP_pos)); }while(0)
#define TIM9_REMAP(val) 	do{ PM_BITMASK(AFIO->PCFR2, (0b11<<3), (val>>TIM_REMAP_pos)); }while(0)
#define TIM10_REMAP(val)	do{ PM_BITMASK(AFIO->PCFR2, (0b11<<5), (val>>TIM_REMAP_pos)); }while(0)


#endif
