#include "usb.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"

USBD_HandleTypeDef hUsbDeviceFS;


void usb_init(usbd_cdc_callback_t callback) {
	USBD_CDC_SetReceiveCallback(callback);
	USBD_Init(&hUsbDeviceFS, &FS_Desc, 0);
	USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
	USBD_Start(&hUsbDeviceFS);
	HAL_NVIC_EnableIRQ(USB_IRQn);
}

bool usb_transmit(const uint8_t *data, uint16_t length) {
	return USBD_CDC_TransmitPacket(&hUsbDeviceFS, data, length) == USBD_OK;
}
