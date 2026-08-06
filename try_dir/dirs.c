#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <sys/sysmacros.h>
#include <sys/statvfs.h>

#include <stdint.h>

#include "./usb/usb.h"
#include "./mem_pool/mem_pool.h"
#include "./path_utils/path_utils.h"
#include "./dir_walk/dir_walk.h"
#include "./backup/backup.h"
#include "./mkdir/my_mkdir.h"


int main(void)
{
	char cwd[PATH_MAX] = {0};
	char *path = "/home/kazuy/ws/usb";
	setHome(cwd, path);	

	usb_t f;
	if(!usbInit(&f))
		exit(1);

	idxPool_t pool;
	idxPoolInit(&pool, f.capacity);

	
	int check = makedir(f.cwdUsb, 0777);
	if(check == EFAULT)
			exit(EXIT_FAILURE);
	
		

	openDir(cwd, " ", &f,ONEDAY, &pool);

	startBackUp(&f, cwd, "/mnt/usb/copied", &pool);

	freedata(&f);

	return 0;
}
