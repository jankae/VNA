/*
 * usb.h
 *
 *  Created on: Aug 12, 2020
 *      Author: jan
 */

#ifndef DRIVERS_USB_USB_H_
#define DRIVERS_USB_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "usbd_cdc.h"

void usb_init(usbd_cdc_callback_t callback);
bool usb_transmit(const uint8_t *data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_USB_USB_H_ */
