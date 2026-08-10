#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include <linux/limits.h>
#include "../file_stat/file_stat.h"
#include <stdbool.h>
#include <sys/statvfs.h>
#include "usb.h"
#include <stdbool.h>
#include <stdio.h>
#include "../path_utils/path_utils.h"
#include "../stack/stack.h"
#include <string.h>


bool isUsbMounted()
{


	struct statvfs vfs;
	if(statvfs("/mnt/usb", &vfs) == -1)
	{
		perror("statvfs");
		return false;
	}
	return true;

}

bool usbInit(usb_t *u)
{
	if(!isUsbMounted())
		return false;
	u->byteWritten = 0;
	u->capacity = MAX_SIZE;
	u->count = 0;
	memset(u->cwdUsb, 0, PATH_MAX);
	setHome(u->cwdUsb, "/mnt/usb/copied");	
	u->files =  calloc(sizeof(file_t),  u->capacity);
	u->limit = getAvailability("/mnt/usb");

	return true;

}


uint64_t getAvailability(const char *path)
{
	struct statvfs vfs;
	if(statvfs(path, &vfs) == -1)
	{
		perror("statvfs");
		return 0;
	}

	//unsigned long long total = (unsigned long long) vfs.f_blocks * vfs.f_frsize;
	uint64_t free = (uint64_t) vfs.f_bfree * vfs.f_frsize;
	//unsigned long long avail = (unsigned long long) vfs.f_bavail * vfs.f_frsize;

//	printf("Total: %llu bytes\n", total);
//	printf("Free: %lu bytes\n", free);
//	printf("avail: %llu bytes\n", avail);
	return free;

}


