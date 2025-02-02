#ifndef __DMA_H__
#define __DMA_H__

#if 1==0
//#define DMA_REV //если входной/выходной канал для данного DMA и данной периферии недоступны, можно попробовать поменять
  dma_clock(DMA, en)
  dma_cfg_io(DMA, dst, src, count);
  dma_cfg_mem(DMA, dst_size, dst_inc, src_size, src_inc, circ, prior);
    //inc = 1 => auto increment, 0 => no auto increment
    //circ = 1 => circular mode, 0 => single mode
    //prior = DMA_PRI_LOW, DMA_PRI_MED, DMA_PRI_HIGH, DMA_PRI_VHIGH
  dma_flag(DMA, flag) //flag may be DMA_F_ERR, DMA_F_FULL, DMA_F_HALF or DMA_F_GLOBAL
  dma_flag_clear(DMA, DMA_F_ERR, DMA_F_FULL, DMA_F_GLOBAL, DMA_F_HALF);
    //DMA_F_ERR - error flag
    //DMA_F_FULL - full transfer flag
    //DMA_F_HALF - half transfer flag
    //DMA_F_GLOBAL - global flag
  dma_interrupts(DMA, DMA_F_FULL, DMA_F_HALF, DMA_F_ERR);
  dma_interrupts_disable(DMA);
  dma_IRQ(DMA){...}
  dma_enable(DMA);
  dma_disable(DMA);
  
//example:
#define SPI1_DMA_TX	1,3,do{SPI1->CTLR2 |= SPI_CTLR2_TXDMAEN;}while(0),do{SPI1->CTLR2 &=~ SPI_CTLR2_TXDMAEN;}while(0)
  void dma_register(SPI1_DMA_TX){}
  dma_clock( SPI1_DMA_TX, 1 );
  dma_cfg_io( SPI1_DMA_TX, &SPI_DATA(1), buf, 100 );
  dma_cfg_mem( SPI1_DMA_TX, 8,0, 8,1, 0, DMA_PRI_LOW );
  dma_enable( SPI1_DMA_TX );
#endif
  
  
///////////////////////////////
  
#include "pinmacro.h"

#define DMA_PRI_LOW		0b00
#define DMA_PRI_MED		0b01
#define DMA_PRI_HIGH	0b10
#define DMA_PRI_VHIGH	0b11
  
#define DMA_PRI_M2M		0b100
  
#define DMA_F_ERR	TE
#define DMA_F_HALF	HT
#define DMA_F_FULL	TC
#define DMA_F_GLOBAL	G
  
#ifndef RCC_DMA2EN //Очередные костыли. Китайцы забыли прописать этот бит в своих хедерах
  #define RCC_DMA2EN	(1<<1)
#endif

#define _DMAx(n, c, ...) DMA ## n
#define _DMA_CH(n, c, ...) DMA ## n ## _Channel ## c
#define _DMA_IRQn(n, c, ...)  DMA ## n ## _Channel ## c ## _IRQn
#define _DMA_AHBEN(n, c, ...) RCC_DMA ## n ## EN
#define DMA_INTFRF(flag, n, c, ...) DMA_ ## flag ## IF ## c
#define DMA_INTFCR(n, c, flag) DMA_C ## flag ## IF ## c
#define DMA_IEN(n, c, flag) DMA_CFGR1_ ## flag ## IE
#define SIZE2BITS(x) ((((x)==8)*0b00)|(((x)==16)*0b01)|(((x)==32)*0b10))

#define DMAx(dma) _DMAx(dma)
#define DMA_CH(dma) _DMA_CH(dma)
#define DMA_IRQn(dma) _DMA_IRQn(dma)
#define DMA_AHBEN(dma) _DMA_AHBEN(dma)

#define _DMA_CLEAR1(f1, n, c, ...) do{_DMAx(n,c)->INTFCR |= DMA_INTFCR(n, c, f1);}while(0)
#define _DMA_CLEAR2(f1, f2, n, c, ...) do{_DMAx(n,c)->INTFCR |= DMA_INTFCR(n, c, f1) | DMA_INTFCR(n, c, f2);}while(0)
#define _DMA_CLEAR3(f1, f2, f3, n, c, ...) do{_DMAx(n,c)->INTFCR |= DMA_INTFCR(n, c, f1) | DMA_INTFCR(n, c, f2) | DMA_INTFCR(n, c, f3);}while(0)
#define _DMA_CLEAR4(f1, f2, f3, f4, n, c, ...) do{_DMAx(n,c)->INTFCR |= DMA_INTFCR(n, c, f1) | DMA_INTFCR(n, c, f2) | DMA_INTFCR(n, c, f3) | DMA_INTFCR(n, c, f4);}while(0)

#define _DMA_INTR_comm(n, c, mask) do{\
  _DMA_CH(n, c)->CFGR= (_DMA_CH(n, c)->CFGR &~ (DMA_CFGR1_TEIE | DMA_CFGR1_HTIE | DMA_CFGR1_TCIE)) | mask;\
  NVIC_EnableIRQ( _DMA_IRQn(n, c) ); \
}while(0)
#define _DMA_INTR1(f1, n, c, ...) _DMA_INTR_comm(n, c, DMA_IEN(n, c, f1) )
#define _DMA_INTR2(f1, f2, n, c, ...) _DMA_INTR_comm(n, c, DMA_IEN(n, c, f1) | DMA_IEN(n, c, f2) )
#define _DMA_INTR3(f1, f2, f3, n, c, ...) _DMA_INTR_comm(n, c, DMA_IEN(n, c, f1) | DMA_IEN(n, c, f2) | DMA_IEN(n, c, f3) )
#define __DMA_INTR(n, c, num) _DMA_INTR ## num
#define _DMA_INTERRUPT(num, n, c, ...) __DMA_INTR(n, c, num)


#define __cnt9_res(a0, a1, a2, a3, a4, a5, a6, a7, a8, res, ...) res
#define _cnt9_res(x) __cnt9_res(x)
#define _cnt9_pre(x...) x,9,8,7,6,5,4,3,2,1,0
#define cnt9(x...) _cnt9_res( _cnt9_pre(x))
#define __DMA_CLEAR(n, c, num) _DMA_CLEAR ## num
#define _DMA_CLEAR(num, n, c, ...) __DMA_CLEAR(n, c, num)

#define dma_clock(dma, x) \
  do{ \
    if(x)RCC->AHBPCENR |= _DMA_AHBEN(dma); else RCC->AHBPCENR &=~ _DMA_AHBEN(dma); \
  }while(0)

#ifndef DMA_REV
  
#define dma_cfg_io(dma, dst, src, cnt) \
  do{ \
    _DMA_CH(dma)->PADDR = (uint32_t)(src); \
    _DMA_CH(dma)->MADDR = (uint32_t)(dst); \
    _DMA_CH(dma)->CNTR  = (uint16_t)(cnt); \
    if(0)(( void(*)(volatile void*, volatile void*) )\
      "dma_cfg_io(void*, void*)")(dst, src); \
  }while(0)
#define dma_cfg_mem(dma, dstsize, dstinc, srcsize, srcinc, circ, prior) do{ \
    uint32_t temp = _DMA_CH(dma)->CFGR; \
    temp &=~ ((0b11*DMA_CFGR1_PL_0) | (0b11*DMA_CFGR1_MSIZE_0) | (0b11*DMA_CFGR1_PSIZE_0) | DMA_CFGR1_MINC | DMA_CFGR1_PINC | DMA_CFGR1_CIRC); \
    temp |= (prior*DMA_CFGR1_PL_0) | (SIZE2BITS(dstsize)*DMA_CFGR1_MSIZE_0) | (SIZE2BITS(srcsize)*DMA_CFGR1_PSIZE_0) | (dstinc*DMA_CFGR1_MINC) | (srcinc*DMA_CFGR1_PINC) | (circ*DMA_CFGR1_CIRC); \
    temp &=~ DMA_CFGR1_DIR; \
    _DMA_CH(dma)->CFGR = temp; \
  }while(0)
  
#else
  
#define dma_cfg_io(dma, dst, src, cnt) \
  do{ \
    _DMA_CH(dma)->PADDR = (uint32_t)(dst); \
    _DMA_CH(dma)->MADDR = (uint32_t)(src); \
    _DMA_CH(dma)->CNTR  = (uint16_t)(cnt); \
  }while(0)
#define dma_cfg_mem(dma, dstsize, dstinc, srcsize, srcinc, circ, prior) do{ \
    uint32_t temp = _DMA_CH(dma)->CFGR; \
    temp &=~ ((0b11*DMA_CFGR1_PL_0) | (0b11*DMA_CFGR1_MSIZE_0) | (0b11*DMA_CFGR1_PSIZE_0) | DMA_CFGR1_MINC | DMA_CFGR1_PINC | DMA_CFGR1_CIRC); \
    temp |= (prior*DMA_CFGR1_PL_0) | (SIZE2BITS(srcsize)*DMA_CFGR1_MSIZE_0) | (SIZE2BITS(dstsize)*DMA_CFGR1_PSIZE_0) | (srcinc*DMA_CFGR1_MINC) | (dstinc*DMA_CFGR1_PINC) | (circ*DMA_CFGR1_CIRC); \
    temp |= DMA_CFGR1_DIR;
    _DMA_CH(dma)->CFGR = temp; \
  }while(0)
  
#endif
  
#define dma_flag(dma, flag) (_DMAx(dma)->INTFR & DMA_INTFRF(flag, dma))
#define dma_flag_clear(dma, flag...) _DMA_CLEAR(cnt9(flag), dma)(flag, dma)
#define dma_interrupts(dma, flag...) do{\
    _DMA_INTERRUPT(cnt9(flag), dma )(flag, dma); \
    NVIC_EnableIRQ( _DMA_IRQn(dma) ); \
  }while(0)
#define dma_interrupts_disable(dma) do{ \
    _DMA_CH(dma)->CFGR &=~ (DMA_CFGR1_TEIE | DMA_CFGR1_HTIE | DMA_CFGR1_TCIE);\
    NVIC_DisableIRQ( _DMA_IRQn(dma) ); \
  }while(0)
#define dma_enable(dma) do{ _DMA_CH(dma)->CFGR |= DMA_CFGR1_EN; _marg3(dma); }while(0)
#define dma_disable(dma) do{ _DMA_CH(dma)->CFGR &=~ DMA_CFGR1_EN; _marg4(dma); }while(0)
  
#define _dma_intr_handler(num, ch, ...) __attribute__((interrupt)) DMA ## num ##  _Channel ## ch ## _IRQHandler
#define dma_intr_handler(dma) _dma_intr_handler(dma)
  
#define _dma_register(num, ch, ...) dma_register_func_dma_ ## num ## _ch_ ## ch()
#define dma_register(dma) _dma_register(dma)

#endif
