#ifndef USB_H
#define USB_H

#include <linux/limits.h>
#include "../file_stat/file_stat.h"
#include <stdint.h>
#include <stdbool.h>


typedef struct 
{
	file_t *files;
	size_t count;
	uint16_t capacity;
	uint64_t byteWritten;
	uint64_t limit;
	char cwdUsb[PATH_MAX];
}usb_t;



bool usbInit(usb_t *u);

bool isUsbMounted();

uint64_t getAvailability(const char *path);

#endif
