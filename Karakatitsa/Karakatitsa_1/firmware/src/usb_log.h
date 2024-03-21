#ifndef _USB_LOG_H_
#define _USB_LOG_H_

#include "usb_lib.h"
#include "hardware.h"

#define USB_LOG_SIZE_PWR	10

char usb_log_ep0_in(config_pack_t *req, void **data, uint16_t *size);
char usb_log_ep0_out(config_pack_t *req, uint16_t offset, uint16_t rx_size);
void usb_log_init();
void usb_log_poll();

void usb_log_grab(uint8_t dir_tx, uint8_t *buf, int size);

#endif