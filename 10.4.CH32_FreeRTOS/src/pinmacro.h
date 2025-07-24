#ifndef __PINMACRO_H__
#define __PINMACRO_H__

#define PM_BITMASK( reg, mask, val ) do{ (reg) = ((reg) &~ (mask)) | (((mask) &~((mask)<<1))*(val)); }while(0)

#define GPIO_OUT  0b0000 //output
#define GPIO_ALT  0b1000 //alternative function output
#define GPIO_PP   0b0000 //push-pull
#define GPIO_OD   0b0100 //open-drain
#define GPIO_2M   0b0010 //max freq = 2 MHz
#define GPIO_10M  0b0001 //max freq = 10 MHz
#define GPIO_50M  0b0011 //max freq = 50 MHz
//basic functions
#define GPIO_PP2  0b0010
#define GPIO_PP10 0b0001
#define GPIO_PP50 0b0011
#define GPIO_OD2  0b0110
#define GPIO_OD10 0b0101
#define GPIO_OD50 0b0111
//alternative functions
#define GPIO_APP2  0b1010
#define GPIO_APP10 0b1001
#define GPIO_APP50 0b1011
#define GPIO_AOD2  0b1110
#define GPIO_AOD10 0b1101
#define GPIO_AOD50 0b1111
//input
#define GPIO_ADC  0b0000
#define GPIO_HIZ  0b0100
#define GPIO_PULL 0b1000

#define concat2(a,b,...)  a##b
#define concat3(a,b,c,...)  a#b#c

#define _marg1(a,...)  a
#define marg1(x) _marg1(x)
#define _marg2(a,b,...)  b
#define marg2(x) _marg2(x)
#define _marg3(a,b,c,...)  c
#define marg3(x) _marg3(x)
#define _marg4(a,b,c,d,...)  d
#define marg4(x) _marg4(x)
#define _marg5(a,b,c,d,e,...)  e
#define marg5(x) _marg5(x)
#define GPIO(x) concat2(GPIO,x)

#define _GPIO_CONFIG(port, bit, ctl, mode) \
  do{\
    uint32_t temp = ctl; \
    temp &=~(0b1111<<(((bit)&0x07)*4)); \
    temp |= ( mode <<(((bit)&0x07)*4)); \
    ctl = temp; \
  }while(0)
  
#define GPIO_mode(port, bit, mode)\
  do{ \
    if(bit<8)_GPIO_CONFIG(port, bit, GPIO(port)->CFGLR, mode); \
      else _GPIO_CONFIG(port, bit-8, GPIO(port)->CFGHR, mode); \
  }while(0)
  

#define GPIO_config(descr) \
  do{ \
    GPIO_mode(_marg1(descr), _marg2(descr), _marg4(descr)); \
    if(_marg4(descr) == GPIO_PULL){ \
      GPIO(_marg1(descr))->BSHR = (1<<_marg2(descr)<<((_marg3(descr))*16)); \
    } \
  }while(0)
  
#define GPIO_manual(descr, mode) \
  do{ \
    GPIO_mode(_marg1(descr), _marg2(descr), mode); \
    if(mode == GPIO_PULL){ \
      GPIO(_marg1(descr))->BSHR = (1<<_marg2(descr)<<((_marg3(descr))*16)); \
    } \
  }while(0)
  
  
#define GPO_ON(descr) \
  do{ \
    GPIO(_marg1(descr))->BSHR = (1<<_marg2(descr)<<((1-_marg3(descr))*16)); \
  }while(0)
  
#define GPO_OFF(descr) \
  do{ \
    GPIO(_marg1(descr))->BSHR = (1<<_marg2(descr)<<((_marg3(descr))*16)); \
  }while(0)
  
#define GPO_T(descr) \
  do{ \
    if(GPIO(_marg1(descr))->OUTDR & (1<<_marg2(descr)) ){\
      GPIO(_marg1(descr))->BCR=(1<<_marg2(descr));\
    }else{\
      GPIO(_marg1(descr))->BSHR=(1<<_marg2(descr));\
    }\
  }while(0)
  
#define GPI_ON(descr)  ((GPIO(_marg1(descr))->INDR & (1<<_marg2(descr)))==(_marg3(descr)<<_marg2(descr)))

#define GPI_OFF(descr) ((GPIO(_marg1(descr))->INDR & (1<<_marg2(descr)))!=(_marg3(descr)<<_marg2(descr)))

#endif
