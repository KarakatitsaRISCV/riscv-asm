#ifndef _CLASS_HID_H_
#define _CLASS_HID_H_

#include "usb_hid.h"

#define HIDREQ_GET_REPORT     1
#define HIDREQ_SET_REPORT     9

typedef volatile struct hid_data_s{
  uint16_t data;
}hid_data_t;
hid_data_t hid_data;

#define HID_TIMEOUT_MS 1000
static volatile uint32_t hid_timeout = 0;

static const uint8_t USB_HIDDescriptor[] = {
  USAGE_PAGE16(0xFF00),
  USAGE(0x01),
  COLLECTION( COLL_APPLICATION,
    LOGICAL_MINMAX(0, 255),
    REPORT_SIZE(8, 8),
    REPORT_COUNT(8, sizeof(hid_data_t)),
    USAGE( 0 ),
    FEATURE_HID16( HID_DATA |  HID_VAR | HID_ABS | HID_BUF ),
  )
};

char hid_ep0_in(config_pack_t *req, void **data, uint16_t *size){
  if( req->bmRequestType == (USB_REQ_INTERFACE | 0x80) ){
    if( req->bRequest == GET_DESCRIPTOR ){
      if( req->wValue == HID_REPORT_DESCRIPTOR){
        *data = (void**)&USB_HIDDescriptor;
        *size = sizeof(USB_HIDDescriptor);
        return 1;
      }
    }
  }
  static hid_data_t resdata = {.data=0};
  if( req->bmRequestType == (USB_REQ_CLASS | USB_REQ_INTERFACE | 0x80) ){
    if( req->bRequest == HIDREQ_GET_REPORT ){
      resdata.data = adc_val;
      *data = (void*)&resdata;
      *size = sizeof(hid_data_t);
      return 1;
    }
  }
  return 0;
}

char hid_ep0_out(config_pack_t *req, uint16_t offset, uint16_t rx_size){
  if( req->bmRequestType == (USB_REQ_CLASS | USB_REQ_INTERFACE) ){
    if( req->bRequest == HIDREQ_SET_REPORT ){
      if(rx_size == 0)return 1; //прочитан только запрос - не интересно
      //анализом запроса не заморачиваемя. Считаем, что ничего кроме нужного не придет
      if(rx_size != sizeof(hid_data))return 0; //защита если вдруг придет мусор
      usb_ep_read(0, (void*)&hid_data);
      
      if(hid_data.data & (1<<0 ))GPO_ON( KBD_0  ); else GPO_OFF( KBD_0  );
      if(hid_data.data & (1<<1 ))GPO_ON( KBD_1  ); else GPO_OFF( KBD_1  );
      if(hid_data.data & (1<<2 ))GPO_ON( KBD_2  ); else GPO_OFF( KBD_2  );
      if(hid_data.data & (1<<3 ))GPO_ON( KBD_3  ); else GPO_OFF( KBD_3  );
      if(hid_data.data & (1<<4 ))GPO_ON( KBD_4  ); else GPO_OFF( KBD_4  );
      if(hid_data.data & (1<<5 ))GPO_ON( KBD_5  ); else GPO_OFF( KBD_5  );
      if(hid_data.data & (1<<6 ))GPO_ON( KBD_6  ); else GPO_OFF( KBD_6  );
      if(hid_data.data & (1<<7 ))GPO_ON( KBD_7  ); else GPO_OFF( KBD_7  );
      
      if(hid_data.data & (1<<8 ))GPO_ON( KBD_8  ); else GPO_OFF( KBD_8  );
      if(hid_data.data & (1<<9 ))GPO_ON( KBD_9  ); else GPO_OFF( KBD_9  );
      if(hid_data.data & (1<<10))GPO_ON( KBD_10 ); else GPO_OFF( KBD_10 );
      if(hid_data.data & (1<<11))GPO_ON( KBD_11 ); else GPO_OFF( KBD_11 );
      if(hid_data.data & (1<<12))GPO_ON( KBD_12 ); else GPO_OFF( KBD_12 );
      if(hid_data.data & (1<<13))GPO_ON( KBD_13 ); else GPO_OFF( KBD_13 );
      if(hid_data.data & (1<<14))GPO_ON( KBD_14 ); else GPO_OFF( KBD_14 );
      if(hid_data.data & (1<<15))GPO_ON( KBD_15 ); else GPO_OFF( KBD_15 );
      
      GPO_ON( BLED );
      hid_timeout = systick_ms() + HID_TIMEOUT_MS;
      
      return 1;
    }
  }
  return 0;
}

void hid_init(){
  jtag_disable();
  GPIO_config( KBD_0  ); GPIO_config( KBD_1  ); GPIO_config( KBD_2  ); GPIO_config( KBD_3  );
  GPIO_config( KBD_4  ); GPIO_config( KBD_5  ); GPIO_config( KBD_6  ); GPIO_config( KBD_7  );
  GPIO_config( KBD_8  ); GPIO_config( KBD_9  ); GPIO_config( KBD_10 ); GPIO_config( KBD_11 );
  GPIO_config( KBD_12 ); GPIO_config( KBD_13 ); GPIO_config( KBD_14 ); GPIO_config( KBD_15 );
  
  GPO_OFF( KBD_0  ); GPO_OFF( KBD_1  ); GPO_OFF( KBD_2  ); GPO_OFF( KBD_3  );
  GPO_OFF( KBD_4  ); GPO_OFF( KBD_5  ); GPO_OFF( KBD_6  ); GPO_OFF( KBD_7  );
  GPO_OFF( KBD_8  ); GPO_OFF( KBD_9  ); GPO_OFF( KBD_10 ); GPO_OFF( KBD_11 );
  GPO_OFF( KBD_12 ); GPO_OFF( KBD_13 ); GPO_OFF( KBD_14 ); GPO_OFF( KBD_15 );
}

void hid_poll(){
  if(systick_ms() > hid_timeout){
    hid_timeout = 0xFFFFFFFF;
    GPO_OFF(BLED);
  }
}

#endif
