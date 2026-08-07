#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "../usb/usb.h"

#include <sys/statvfs.h>

#include "../stack/stack.h"

typedef struct
{
	Stack idxAvailable;
	bool idxInUse[MAX_SIZE];
	size_t count;

}idxPool_t;


void freedata(usb_t *f, idxPool_t *p);

bool isAboveLimit(uint64_t byteRead, uint64_t limit, uint64_t currByte);
void *myRealloc(void *old, size_t newSize);

uint64_t getAvailability(const char *path);

void idxPoolInit(idxPool_t *p, uint16_t capacityUsb );
#endif
