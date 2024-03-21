#include "usb_log.h"

#define USB_LOG_SIZE (1<<USB_LOG_SIZE_PWR)
#define USB_LOG_MASK (USB_LOG_SIZE-1)
volatile struct{
  volatile uint16_t st,en;
  volatile uint8_t arr[USB_LOG_SIZE];
}fifo;

uint16_t fifo_data_size(){
  return ((fifo.en - fifo.st) & USB_LOG_MASK);
}
#define fifo_free_size() (USB_LOG_MASK - fifo_data_size())

void fifo_write(uint8_t *buf, int size){
  uint16_t en = fifo.en;
  uint16_t av = fifo_free_size();
  if(av > size)av = size;
  while( av > 0 ){
    fifo.arr[en] = buf[0];
    en = (en + 1) & USB_LOG_MASK;
    buf++;
    av--;
  }
  fifo.en = en;
}
#define fifo_str(str) fifo_write((uint8_t*)str, sizeof(str))

int fifo_read(uint8_t *buf, int size){
  uint16_t av = fifo_data_size();
  uint16_t st = fifo.st;
  if(av > size)av = size;
  size = av;
  while( av > 0 ){
    buf[0] = fifo.arr[st];
    buf++;
    st = (st + 1) & USB_LOG_MASK;
    av--;
  }
  fifo.st = st;
  return size;
}

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define CDC_SEND_ENCAPSULATED 0x00
#define CDC_GET_ENCAPSULATED  0x01
#define CDC_SET_COMM_FEATURE  0x02
#define CDC_GET_COMM_FEATURE  0x03
#define CDC_CLR_COMM_FEATURE  0x04
#define CDC_SET_LINE_CODING   0x20
#define CDC_GET_LINE_CODING   0x21
#define CDC_SET_CTRL_LINES    0x22
#define CDC_SEND_BREAK        0x23

struct cdc_linecoding{
  uint32_t baudrate;
  uint8_t stopbits; //0=1bit, 1=1.5bits, 2=2bits
  uint8_t parity; //0=none, 1=odd, 2=even, 3=mark (WTF?), 4=space (WTF?)
  uint8_t wordsize; //length of data word: 5,6,7,8 or 16 bits
}__attribute__((packed));

volatile struct cdc_linecoding log_cfg = {
  .baudrate = 9600,
  .stopbits = 1,
  .parity = 0,
  .wordsize = 8,
};

char usb_log_ep0_in(config_pack_t *req, void **data, uint16_t *size){
  if( (req->bmRequestType & 0x7F) == (USB_REQ_CLASS | USB_REQ_INTERFACE) ){
    if( req->bRequest == CDC_GET_LINE_CODING ){
      if( req->wIndex == ifnum(interface_log) ){
        *data = (void*)&log_cfg;
        *size = sizeof(log_cfg);
        return 1;
      }
    }
  }
  return 0;
}

char usb_log_ep0_out(config_pack_t *req, uint16_t offset, uint16_t rx_size){
  if( (req->bmRequestType & 0x7F) == (USB_REQ_CLASS | USB_REQ_INTERFACE) ){
    if( req->bRequest == CDC_SET_LINE_CODING ){
      if(rx_size == 0)return 1;
      if( req->wIndex == ifnum(interface_tty) ){
        usb_ep_read(0, (void*)&log_cfg);
        return 1;
      }
    }
  }
  return 0;
}
void usb_log_init(){
  usb_ep_init( ENDP_LOG_CTL  | 0x80, USB_ENDP_INTR, ENDP_CTL_SIZE,  NULL );
  usb_ep_init( ENDP_LOG_IN   | 0x80, USB_ENDP_BULK, ENDP_LOG_SIZE,  NULL );
  usb_ep_init( ENDP_LOG_OUT,         USB_ENDP_BULK, ENDP_LOG_SIZE,  NULL );
}

void log_dir(uint8_t dir_tx){
  if(dir_tx){
    fifo_str("⍄");
  }else{
    fifo_str("⍃");
  }
}

void usb_log_grab(uint8_t dir_tx, uint8_t *buf, int size){
  static uint8_t dir_prev = 0xFF;
  uint16_t av = fifo_free_size();
  if(av < 10)return;
  
  if(dir_prev != dir_tx){
    dir_prev = dir_tx;
    fifo_str("\r\n");
    log_dir(dir_tx);
  }
  uint16_t i, pi = 0;
  for(i=0; i<size; i++){
    if(buf[i] == '\r'){
      fifo_write(&buf[pi], i-pi);
      fifo_str("\r");
      log_dir(dir_tx);
    }else if(buf[i] == '\n'){
      fifo_write(&buf[pi], i-pi);
      fifo_str("\n\r");
      log_dir(dir_tx);
    }else{
      continue;
    }
    
    pi = i+1;
  }
  fifo_write(&buf[pi], i-pi);
}

void usb_log_poll(){
  uint16_t size;
  USB_ALIGN uint8_t buf[ENDP_LOG_SIZE];
  if(usb_ep_ready( ENDP_LOG_IN | 0x80 )){
    size = fifo_read(buf, sizeof(buf));
    usb_ep_write( ENDP_LOG_IN | 0x80, (uint16_t*)buf, size );
  }
  if(usb_ep_ready( ENDP_LOG_OUT ) ){
    usb_ep_read( ENDP_LOG_OUT, (uint16_t*)buf);
  }
}