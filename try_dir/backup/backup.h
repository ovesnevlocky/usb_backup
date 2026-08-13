#ifndef BACKUP_H
#define BACKUP_H


#include <stdint.h>
#include <stdbool.h>
#include "../mem_pool/mem_pool.h"
#include "../usb/usb.h"


enum
{
	ABOVE_LIMIT_USB = -100,
	FAILURE_IN_WRITING,
	ERROR_OPEN_F,
	ERROR_CLOSE_F,
	INVALID_PATH, 

};
void startBackUp(usb_t *f, const char *home, char *usbHome, idxPool_t *p);


//copy a file, returns  bytes written  if success, otherwise 0
int64_t copyFile(const char *cwd, const  char *saveTo, const usb_t * u);
bool isAlreadyCopied(char *pathUsb);

void checkFiles(usb_t *f, const uint32_t period, idxPool_t *p);
#endif
