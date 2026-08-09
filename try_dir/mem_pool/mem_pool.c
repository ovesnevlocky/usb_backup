#include "mem_pool.h"

#include "../stack/stack.h"
#include "../usb/usb.h"

#include <stdio.h>
#include <string.h>

void freedata(usb_t *f, idxPool_t *p)
{
	uint32_t count = p->count;
	int idx = 0;
	while(count > 0)
	{
		if(p->idxInUse[idx] == true)
		{
					
			free(f->files[idx].pathOriginal);
			free(f->files[idx].pathUsb);

			f->files[idx].pathOriginal = NULL;
			f->files[idx].pathUsb = NULL;
			count--;
		}

		idx++;
	}

	free(p->idxInUse);
	p->idxInUse = NULL;
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


void pushFreeIdx(idxPool_t *p, uint16_t idxs, uint16_t offset)
{

	while(offset > 0)
	{
		stackPush(&p->idxAvailable, (int)idxs - 1);
		p->idxInUse[idxs - 1] = false;
		idxs--;
		offset--;
	}

	return;	

}

void idxPoolInit(idxPool_t *p, uint16_t capacityUsb)
{
	stackInit(&p->idxAvailable);
	p->idxInUse = malloc(MAX_SIZE * sizeof(bool));
	if(!p->idxInUse)
	{
		perror("malloc");
		return;
	}
	p->count = 0;
	//push from 99 as that is kind of natural i guess.. doesnt really matter	
	pushFreeIdx(p, capacityUsb, capacityUsb);

	return;
}

bool isAboveLimit(uint64_t byteRead, uint64_t limit, uint64_t currByte)
{
	return byteRead + currByte >= limit;
}
