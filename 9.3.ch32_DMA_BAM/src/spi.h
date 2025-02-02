//#ifndef __SPI_H__
//#define __SPI_H__

#if 0==1
#define SPIn			1 //if macro SPIn not defined -> software SPI
#define SPI_MISO		B,4,1 //may be undefined
#define SPI_MOSI		B,5,1 //may be undefined
#define SPI_SCK			B,3,1
//#define SPI_REMAP
#define SPI_SPEED_DIV	2 // F_APBx / 2..256
//SPI_soft_speed 123  //(software SPI only!) Delay between bits. For super-slow SPI
#define SPI_LSBFIRST	0
#define SPI_PHASE		0
#define SPI_MODE		SPI_MASTER

static inline void SPI_init();
static uint16_t SPI_send(uint16_t data);
uint8_t SPI_ready();
void SPI_disable();
void SPI_enable();
static inline void SPI_size_8();
static inline void SPI_size_16();

#endif

#define SPI_MASTER	1
#define SPI_SLAVE	0

#define _SPI_NAME(num) SPI ## num
#define SPI_NAME(num) _SPI_NAME(num)

#define SPI_DATA(num) (SPI_NAME(num)->DATAR)

#define _SPI_init(spi) SPI ## spi ## _init()
#define _SPI_ready(spi) SPI ## spi ## _ready()
#define _SPI_wait(spi) SPI ## spi ## _wait()
#define _SPI_enable(spi) SPI ## spi ## _enable()
#define _SPI_disable(spi) SPI ## spi ## _disable()
#define _SPI_size_8(spi) SPI ## spi ## _size_8()
#define _SPI_size_16(spi) SPI ## spi ## _size_16()
#define _SPI_send(spi, data) SPI ## spi ## _send(data)
#define _SPI_exch(spi, data) SPI ## spi ## _exch(data)
#define _SPI_DMA_TX(spi) SPI ## spi ## _DMA_TX
#define _SPI_DMA_RX(spi) SPI ## spi ## _DMA_RX


#define SPI_init(spi) _SPI_init(spi)
#define SPI_ready(spi) _SPI_ready(spi)
#define SPI_wait(spi) _SPI_wait(spi)
#define SPI_enable(spi) _SPI_enable(spi)
#define SPI_disable(spi) _SPI_disable(spi)
#define SPI_size_8(spi) _SPI_size_8(spi)
#define SPI_size_16(spi) _SPI_size_16(spi)
#define SPI_send(spi, data) _SPI_send(spi, data)
#define SPI_exch(spi, data) _SPI_exch(spi, data)
#define SPI_DMA_TX(spi) _SPI_DMA_TX(spi)
#define SPI_DMA_RX(spi) _SPI_DMA_RX(spi)

#ifndef SPI_SCK
  //#error not found '#define SPI_SCK P,n,a' (P=port, n=pin number, a=active level)
#endif
#ifndef SPI_LSBFIRST
  #warning SPI_LSBFIRST not defined; using default (0)
  #define SPI_LSBFIRST 0
#endif
#ifndef SPI_PHASE
  #warning SPI_PHASE not defined; using default (0)
  #define SPI_PHASE 0
#endif
#if !defined(SPI_MODE) || !( (SPI_MODE == SPI_MASTER) || (SPI_MODE == SPI_SLAVE) )
  #if SPIn == 1
    #error define SPI_MODE as SPI_MASTER or SPI_SLAVE in SPI1
  #elif SPIn == 2
    #error define SPI_MODE as SPI_MASTER or SPI_SLAVE in SPI2
  #elif SPIn == 3
    #error define SPI_MODE as SPI_MASTER or SPI_SLAVE in SPI3
  #else
    #error define SPI_MODE as SPI_MASTER or SPI_SLAVE in software SPI
  #endif
#endif

#define _SPI_func(n, func) SPI ## n ## _ ## func
#define SPI_func(n, func) _SPI_func(n, func)

#ifndef SPI1_DMA_TX
  #define SPI1_DMA_TX	1,3,do{SPI1->CTLR2 |= SPI_CTLR2_TXDMAEN;}while(0),do{SPI1->CTLR2 &=~ SPI_CTLR2_TXDMAEN;}while(0)
  #define SPI1_DMA_RX	1,2,do{SPI1->CTLR2 |= SPI_CTLR2_RXDMAEN;}while(0),do{SPI1->CTLR2 &=~ SPI_CTLR2_RXDMAEN;}while(0)
#endif
#ifndef SPI2_DMA_TX
  #define SPI2_DMA_TX	1,5,do{SPI2->CTLR2 |= SPI_CTLR2_TXDMAEN;}while(0),do{SPI2->CTLR2 &=~ SPI_CTLR2_TXDMAEN;}while(0)
  #define SPI2_DMA_RX	1,4,do{SPI2->CTLR2 |= SPI_CTLR2_RXDMAEN;}while(0),do{SPI2->CTLR2 &=~ SPI_CTLR2_RXDMAEN;}while(0)
#endif
#ifndef SPI3_DMA_TX
  #define SPI3_DMA_TX	2,2,do{SPI3->CTLR2 |= SPI_CTLR2_TXDMAEN;}while(0),do{SPI3->CTLR2 &=~ SPI_CTLR2_TXDMAEN;}while(0)
  #define SPI3_DMA_RX	2,1,do{SPI3->CTLR2 |= SPI_CTLR2_RXDMAEN;}while(0),do{SPI3->CTLR2 &=~ SPI_CTLR2_RXDMAEN;}while(0)
#endif

#ifndef SPI_DECLARATIONS
////////////// Hardware SPI /////////////////////////////////////////////////
#if defined(SPIn) && (SPIn == 1 || SPIn == 2 || SPIn == 3)

#ifndef SPI_SPEED_DIV
  #warning SPI_SPEED_DIV not defined; using slowest (1:256)
  #define SPI_SPEED_DIV 256
#endif
#if (SPI_SPEED_DIV == 2)
  #define SPI_BRR (0*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 4)
  #define SPI_BRR (1*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 8)
  #define SPI_BRR (2*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 16)
  #define SPI_BRR (3*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 32)
  #define SPI_BRR (4*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 64)
  #define SPI_BRR (5*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 128)
  #define SPI_BRR (6*SPI_CTLR1_BR_0)
#elif (SPI_SPEED_DIV == 256)
  #define SPI_BRR (7*SPI_CTLR1_BR_0)
#else
  #error SPI: wrong SPI_SPEED_DIV macro
#endif

#if SPIn == 1
  #ifndef SPI_REMAP
    #define SPI_do_remap() do{AFIO->PCFR1 &=~AFIO_PCFR1_SPI1_REMAP; }while(0)
  #else
    #define SPI_do_remap() do{AFIO->PCFR1 |= AFIO_PCFR1_SPI1_REMAP; }while(0)
  #endif
#elif SPIn == 2
  #ifndef SPI_REMAP
    #define SPI_do_remap() do{}while(0)
  #else
    #define SPI_do_remap() do{}while(0)
  #endif
#elif SPIn == 3
  #ifndef SPI_REMAP
    #define SPI_do_remap() do{AFIO->PCFR1 &=~(1<<28);}while(0)
  #else
    #define SPI_do_remap() do{AFIO->PCFR1 |= (1<<28);}while(0)
  #endif
#endif

#ifdef SPI_SCK
#define SPI_CHPOL	(!marg3(SPI_SCK))
#else
#define SPI_CHPOL	0
#endif

#define CR1_mstr_def (SPI_CTLR1_MSTR | SPI_BRR | SPI_CTLR1_SPE | SPI_CTLR1_SSI | SPI_CTLR1_SSM | \
                (SPI_CTLR1_LSBFIRST * (SPI_LSBFIRST)) |\
                (SPI_CTLR1_CPHA * (SPI_PHASE)) | \
                (SPI_CTLR1_CPOL * SPI_CHPOL))

#define CR1_sla_def (SPI_BRR | SPI_CTLR1_SPE | SPI_CTLR1_SSM | \
                (SPI_CTLR1_LSBFIRST * (SPI_LSBFIRST)) |\
                (SPI_CTLR1_CPHA * (SPI_PHASE)) | \
                (SPI_CTLR1_CPOL * SPI_CHPOL))

#if SPI_MODE == SPI_MASTER
  #define _SPI_rdy() (!(SPI_NAME(SPIn)->STATR & SPI_STATR_BSY))
#else
  #define _SPI_rdy() (SPI_NAME(SPIn)->STATR & SPI_STATR_RXNE)
#endif

uint8_t SPI_func(SPIn, ready)(){
  return _SPI_rdy();
}

void SPI_func(SPIn, wait)(){
  while( !_SPI_rdy() ){}
}

void SPI_func(SPIn, disable)(){
  SPI_wait(SPIn);
  SPI_NAME(SPIn)->CTLR1 &=~ SPI_CTLR1_SPE;
}

void SPI_func(SPIn, enable)(){
  //SPI_wait(SPIn);
  SPI_NAME(SPIn)->CTLR1 |= SPI_CTLR1_SPE;
}

void SPI_func(SPIn, size_8)(){
  //SPI_wait(SPIn);
  SPI_NAME(SPIn)->CTLR1 &=~ SPI_CTLR1_SPE;
  SPI_NAME(SPIn)->CTLR1 &=~ SPI_CTLR1_DFF;
  SPI_NAME(SPIn)->CTLR1 |= SPI_CTLR1_SPE;
}

void SPI_func(SPIn, size_16)(){
  //SPI_wait(SPIn);
  SPI_NAME(SPIn)->CTLR1 &=~ SPI_CTLR1_SPE;
  SPI_NAME(SPIn)->CTLR1 |= SPI_CTLR1_DFF;
  SPI_NAME(SPIn)->CTLR1 |= SPI_CTLR1_SPE;
}

void SPI_func(SPIn, init)(){
#if SPIn == 1
  RCC->APB2PCENR |= RCC_SPI1EN;
#elif SPIn == 2
  RCC->APB1PCENR |= (1<<14); //RCC_SPI2EN;
#elif SPIn == 3
  RCC->APB1PCENR |= (1<<15); //RCC_SPI3EN;
#endif
  
  SPI_NAME(SPIn)->CTLR1 = 0;
  SPI_do_remap();
  
#if SPI_MODE == SPI_MASTER
#ifdef SPI_MISO
  GPIO_manual(SPI_MISO, GPIO_HIZ);
#endif
#ifdef SPI_MOSI
  GPIO_manual(SPI_MOSI, GPIO_APP50);
  GPO_OFF(SPI_MOSI);
#endif
#ifdef SPI_SCK
  GPIO_manual(SPI_SCK, GPIO_APP50);
  GPO_OFF(SPI_SCK);
#endif

  SPI_NAME(SPIn)->CTLR1 = CR1_mstr_def;
  
#else //SPI_MODE == SPI_SLAVE
  
#ifdef SPI_MISO
  GPIO_manual(SPI_MISO, GPIO_APP50);
  GPO_OFF(SPI_MISO);
#endif
#ifdef SPI_MOSI
  GPIO_manual(SPI_MOSI, GPIO_HIZ);
#endif
#ifdef SPI_SCK
  GPIO_manual(SPI_SCK, GPIO_HIZ);
#endif
  SPI_NAME(SPIn)->CTLR1 = CR1_sla_def;
#endif
  
#if (SPI_SPEED_DIV == 2)
  //SPI_NAME(SPIn)->HSCR = 1;
  SPI_NAME(SPIn)->HSCR = 0b101;
#endif
}

uint16_t SPI_func(SPIn, send)(uint16_t data){
  uint16_t res;
  SPI_wait(SPIn);
  res = SPI_NAME(SPIn)->DATAR;
  SPI_NAME(SPIn)->DATAR = data;
  return res;
}

uint16_t SPI_func(SPIn, exch)(uint16_t data){
  SPI_NAME(SPIn)->DATAR = data;
  SPI_wait(SPIn);
  return SPI_NAME(SPIn)->DATAR;
}

#undef SPI_BRR
#undef CR1_mstr_def
#undef CR1_sla_def
#undef SPI_MISO
#undef SPI_MOSI
#undef SPI_SCK
#undef SPI_SPEED_DIV
#undef SPI_LSBFIRST
#undef SPI_PHASE
#undef SPI_MODE
#undef _SPI_rdy
#undef _SPI_do_remap
#undef SPI_do_remap
#undef SPI_REMAP

#else
//////////////// Software SPI /////////////////////////////////////////
#warning Software SPI

#ifndef SPI_SPEED_DIV
  #ifndef SPI_soft_speed
    #warning SPI_SPEED_DIV not defined; using slowest (1:256)
    #define SPI_SPEED_DIV 256
  #endif
#else
  #define SPI_soft_speed (SPI_SPEED_DIV*4)
#endif

#if SPI_MODE == SPI_SLAVE
  #error Software SPI not implemented yet
#endif
#if SPI_LSBFIRST != 0
  #error Software SPI does not support LSBFIRST yet
#endif
#if SPI_PHASE != 0
  #error Software SPI does not support SPI_PHASE yet
#endif

#define SPI_intr_var(SPIn, name) SPI ## SPIn ## _intr_var_ ## name

uint8_t SPI_intr_var(SPIn, size) = 8;

static void SPI_func(SPIn, sleep)(){
  uint32_t time = SPI_soft_speed;
  while(time--)asm volatile("nop");
}


uint8_t SPI_func(SPIn, ready)(){ return 1;}
void SPI_func(SPIn, wait)(){}
void SPI_func(SPIn, disable)(){}
void SPI_func(SPIn, enable)(){}

void SPI_func(SPIn, size_8)(){SPI_intr_var(SPIn, size) = 8;}
void SPI_func(SPIn, size_16)(){SPI_intr_var(SPIn, size) = 16;}

void SPI_func(SPIn, init)(){
#ifdef SPI_MISO
  GPIO_manual(SPI_MISO, GPIO_HIZ);
#endif
#ifdef SPI_MOSI
  GPIO_manual(SPI_MOSI, GPIO_PP50);
  GPO_OFF(SPI_MOSI);
#endif
  GPIO_manual(SPI_SCK, GPIO_PP50);
  GPO_OFF(SPI_SCK);
}

uint16_t SPI_func(SPIn, send)(uint16_t data){
  if(SPI_intr_var(SPIn, size) == 8)data <<= 8;
  for(int i=0; i<SPI_intr_var(SPIn, size); i++){
    #ifdef SPI_MOSI
    if(data & 0x8000)GPO_ON(SPI_MOSI); else GPO_OFF(SPI_MOSI);
    #endif
    SPI_func(SPIn, sleep)();
    GPO_ON(SPI_SCK);
    data<<=1;
    #ifdef SPI_MISO
    if(GPI_ON(SPI_MISO))data |= (1<<0);
    #endif
    SPI_func(SPIn, sleep)();
    GPO_OFF(SPI_SCK);
  }
  return data;
}

uint16_t SPI_func(SPIn, exch)(uint16_t data){
  return SPI_func(SPIn, send)(data);
}

#undef SPI_MISO
#undef SPI_MOSI
#undef SPI_SCK
#undef SPI_SPEED_DIV
#undef SPI_soft_speed
#undef SPI_LSBFIRST
#undef SPI_PHASE
#undef SPI_MODE

#endif //hardware/software SPI


#else //SPI_DECLARATIONS

void SPI_func(SPIn, init)();
uint8_t SPI_func(SPIn, ready)();
void SPI_func(SPIn, wait)();
void SPI_func(SPIn, disable)();
void SPI_func(SPIn, enable)();
void SPI_func(SPIn, size_8)();
void SPI_func(SPIn, size_16)();
uint16_t SPI_func(SPIn, send)(uint16_t data);
uint16_t SPI_func(SPIn, exch)(uint16_t data);

#endif //SPI_DECLARATIONS

//#endif

#if 1==0

#define SPIn			1
#define SPI_MISO		A,6,1
#define SPI_MOSI		A,7,1
#define SPI_SCK			A,5,1
//Remap
#define SPI_MISO		B,4,1
#define SPI_MOSI		B,5,1
#define SPI_SCK			B,3,1

#define SPIn			2
#define SPI_MISO		B,14,1
#define SPI_MOSI		B,15,1
#define SPI_SCK			B,13,1

#define SPIn			3
#define SPI_MISO		B,4,1
#define SPI_MOSI		B,5,1
#define SPI_SCK			B,3,1
//Remap
#define SPI_MISO		C,11,1
#define SPI_MOSI		C,12,1
#define SPI_SCK			C,10,1

#endif
