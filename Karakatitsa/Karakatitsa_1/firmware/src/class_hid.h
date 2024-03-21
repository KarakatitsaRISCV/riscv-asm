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
      resdata.data = 0;
      //resdata.data = TIM2->ARR; //DEBUG
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
      
      //PA0-PA7 + PB8-PB15 = hid_data
      uint16_t temp = GPIOA->ODR;
      temp = (temp & 0xFF00) | (hid_data.data & 0x00FF);
      GPIOA->ODR = temp;
      
      temp = GPIOB->ODR;
      temp = (temp & 0x00FF) | (hid_data.data & 0xFF00);
      GPIOB->ODR = temp;
      
      //GPO_T( BLED );
      hid_timeout = systick_ms() + HID_TIMEOUT_MS;
      
      return 1;
    }
  }
  return 0;
}

void hid_init(){
  //PA0-PA7 = GPIO_PP50
  GPIOA->MODER = (GPIOA->MODER &~ (0b11 * 0x00005555)) | (GPIO_PP * 0x00005555);
  GPIOA->OSPEEDR = (GPIOA->OSPEEDR &~ (0b11 * 0x00005555)) | (GPIO_VS * 0x00005555);
  //PB8-PB15 = GPIO_PP50
  GPIOB->MODER = (GPIOB->MODER &~ (0b11 * 0x55550000U)) | (GPIO_PP * 0x55550000U);
  GPIOB->OSPEEDR = (GPIOB->OSPEEDR &~ (0b11 * 0x55550000U)) | (GPIO_VS * 0x55550000U);
}

void hid_poll(){
  if(systick_ms() > hid_timeout){
    hid_timeout = 0xFFFFFFFF;
    //GPO_OFF(BLED);
  }
}

#endif
