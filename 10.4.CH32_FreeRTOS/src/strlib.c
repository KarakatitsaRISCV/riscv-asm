#include "strlib.h"

#ifdef MEMFUNC_DMA

#include "dma.h"

#define ch32v20x 203
#define ch32v30x 303

#if marg3(MEMFUNC_DMA)==ch32v20x
  #include "ch32v20x.h"
#elif marg3(MEMFUNC_DMA)==ch32v30x
  #include "ch32v30x.h"
#else
  #error MCU not defined. Use -DMEMFUNC_DMA=1,1,ch32v20x or -DMEMFUNC_DMA=3,2,ch32v30x or same
#endif

#undef ch32v20x
#undef ch32v30x

#if marg1(MEMFUNC_DMA) == 1
  #warning DMA1 cant cross 64k memory boundary
#endif

void dma_register(MEMFUNC_DMA){}

void *memcpy(void *dst, void *src, uint32_t n){
  dma_clock(MEMFUNC_DMA, 1);
  DMA_CH(MEMFUNC_DMA)->CFGR &=~ DMA_CFGR1_EN;
  DMA_CH(MEMFUNC_DMA)->CFGR |= DMA_CFGR1_MEM2MEM;
  
  dma_flag_clear(MEMFUNC_DMA, DMA_F_FULL);
  
  dma_cfg_io(MEMFUNC_DMA, dst, src, n/4);
  switch( ((uint32_t)dst | (uint32_t)src) & 3 ){
    case 0: dma_cfg_mem(MEMFUNC_DMA, 32,1, 32,1, 0, DMA_PRI_LOW); break;
    case 2: dma_cfg_mem(MEMFUNC_DMA, 16,1, 16,1, 0, DMA_PRI_LOW); DMA_CH(MEMFUNC_DMA)->CNTR = n/2; break;
    default:dma_cfg_mem(MEMFUNC_DMA, 8, 1, 8, 1, 0, DMA_PRI_LOW); DMA_CH(MEMFUNC_DMA)->CNTR = n;   break;
  }
  
  DMA_CH(MEMFUNC_DMA)->CFGR |= DMA_CFGR1_EN;
  while(!dma_flag(MEMFUNC_DMA, DMA_F_FULL)){}
  return dst;
}

void *memset(void *dst, int val, uint32_t n){
  dma_clock(MEMFUNC_DMA, 1);
  DMA_CH(MEMFUNC_DMA)->CFGR |= DMA_CFGR1_MEM2MEM;
  if(n > 3){
    __attribute__ ((aligned (4))) uint8_t src[4] = {val, val, val, val};
    DMA_CH(MEMFUNC_DMA)->CFGR &=~ DMA_CFGR1_EN;
    dma_flag_clear(MEMFUNC_DMA, DMA_F_FULL);
    dma_cfg_io(MEMFUNC_DMA, (uint8_t*)(((uint32_t)dst+3)&~3LU), &src, n/4);
    dma_cfg_mem(MEMFUNC_DMA, 32,1, 32,0, 0, DMA_PRI_VHIGH);
    DMA_CH(MEMFUNC_DMA)->CFGR |= DMA_CFGR1_EN;
    
    ((uint8_t*)dst)[0] = ((uint8_t*)dst)[1] = ((uint8_t*)dst)[2] = val;
    ((uint8_t*)dst)[n-1] = ((uint8_t*)dst)[n-2] = ((uint8_t*)dst)[n-3] = val;
    
    while(!dma_flag(MEMFUNC_DMA, DMA_F_FULL)){}
  }else{
    for(int i=0; i<n; i++)((uint8_t*)dst)[i] = val;
  }
  return dst;
}

#elif defined MEMFUNC

void* memcpy(void *dst, void *src, uint32_t len){
  for(uint32_t i=0; i<len; i++){
    ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
  }
  return dst;
}

void *memset(void *s, int c, uint32_t n){
  for(uint32_t i=0; i<n; i++)((uint8_t*)s)[i] = c;
  return s;
}

#endif

#define NUMLEN(val) (\
  (val)<100000?( /* <10⁵ */ \
    (val)< 100?( \
      (val)<10? 1 : 2 \
    ):( /*  10³ / 10⁴ */ \
      (val)<10000? (val)<1000? 3 : 4 : 5 \
    ) \
  ):( /* 10⁶ ... 10¹⁰ */\
    (val) < 10000000?( /* <10⁸ */ \
      (val)<1000000? 6 : 7 \
    ):( /* 10⁸, 10⁹, 10¹⁰ */ \
      (val)<1000000000? (val)<100000000? 8 : 9 : 10 \
    ) \
  ) \
  )

static char strlib_buf[13];

char* utobin(char *buf, uint32_t val, uint8_t bits){
  if(buf == NULL)buf = strlib_buf;
  char *ch = buf;
  for(int i=(1<<(bits-1)); i!=0; i>>=1){
    if(val & i)ch[0]='1'; else ch[0]='0';
    ch++;
  }
  ch[0] = 0;
  return buf;
}

char* fpi32tos(char *buf, int32_t val, uint8_t dot, int8_t field){
  if(buf == NULL)buf = strlib_buf;
  buf[0] = 0;
  char *ch = &buf[13];
  ch--; ch[0] = 0;
  if(val < 0){buf[0]='-'; val=-val;}
  for(int i=0; i<dot; i++){
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }
  if(dot > 0){ch--; ch[0]='.';}
  do{
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }while(val);
  if(buf[0] != 0){ch--; ch[0] = '-';}
  field -= 12-(ch-buf);
  for(;field>0;field--){ch--; ch[0] = ' ';}
  return ch;
}

char* fpi32tos_inplace(char *buf, int32_t val, uint8_t dot, int8_t field){
  static char strlib_buf[13];
  if(buf == NULL)buf = strlib_buf;
  uint32_t len = val;
  if(val < 0)len = -val;
  len = NUMLEN(len);
  if(dot >= len)len = dot + 2; else if(dot > 0)len++;
  if(val < 0)len++;
  
  if( len > field ){
    if( field == 0 ){
      field = len;
    }else{
      if(val > 0){
        for(uint32_t i=0; i<field; i++)buf[i] = '+';
      }else{
        for(uint32_t i=0; i<field; i++)buf[i] = '-';
      }
      return buf;
    }
  }
  uint32_t empty = field - len;
  for(int i=0; i<empty; i++)buf[i] = ' ';
  if(val < 0){val = -val; buf[empty] = '-';}
  
  char *ch = &buf[field];
  
  for(int i=0; i<dot; i++){
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }
  if(dot > 0){ch--; ch[0]='.';}
  do{
    ch--; ch[0] = (val % 10)+'0';
    val /= 10;
  }while(val);

  return &buf[field];
}

char* u32tohex(char *buf, uint32_t val, uint32_t digs){
  if(buf == NULL)buf = strlib_buf;
  if(digs > 8)return NULL;
  val <<= (8-digs)*4;
  for(int i=0; i<digs; i++){
    uint32_t b = val >> 28;
    if(b < 0xA)buf[i] = b+'0'; else buf[i] = b-0xA+'A';
    val <<= 4;
  }
  buf[digs] = 0;
  return buf;
}

#ifdef STRMATCH_FUNC

char* strstr(char *str, char *substr){
  char *cp = (char *)str;
  char *s1, *s2;
  if( !*substr )return((char *)str);
  while(cp[0]!=0){
    s1 = cp;
    s2 = (char *)substr;
    while( s1[0] && s2[0] && (s1[0]==s2[0]) )s1++, s2++;
    if(s2[0]==0)return(cp);
    cp++;
  }
  return(NULL);
}

#endif