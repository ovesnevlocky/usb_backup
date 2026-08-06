#include "mem_pool.h"

#include "../stack/stack.h"
#include "../usb/usb.h"

#include <stdio.h>
#include <string.h>

void freedata(usb_t *f)
{
	for(size_t i = 0; i < f->count; i++)
	{
		free(f->files[i].pathOriginal);
		free(f->files[i].pathUsb);

		f->files[i].pathOriginal = NULL;
		f->files[i].pathUsb = NULL;
	}

	free(f->files);
	f->files = NULL;
}


void *myRealloc(void *old, size_t newSize)
{
	void *new = realloc(old, newSize);
	if(!new)
	{
		perror("realloc");
		return NULL;		
	}

	return new;
}


void idxPoolInit(idxPool_t *p, uint16_t capacityUsb)
{
	stackInit(&p->idxAvailable);
	memset(p->idxInUse, false, MAX_SIZE * sizeof(bool));
	p->count = 0;
	//push from 99 as that is kind of natural i guess.. doesnt really matter	
	while(capacityUsb > 0)
	{
		stackPush(&p->idxAvailable,  (int)capacityUsb - 1);	
		capacityUsb--;	
	}

	return;
}

bool isAboveLimit(uint64_t byteRead, uint64_t limit, uint64_t currByte)
{
	return byteRead + currByte >= limit;
}
