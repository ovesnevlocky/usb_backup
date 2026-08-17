#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/sysmacros.h>
#include <sys/statvfs.h>

#include <stdint.h>

#include "./usb/usb.h"
#include "./mem_pool/mem_pool.h"
#include "./path_utils/path_utils.h"
#include "./dir_walk/dir_walk.h"
#include "./backup/backup.h"
#include "./mkdir/my_mkdir.h"
#include <string.h>

int main(void)
{
	path_t cwd;
	pathInit(&cwd);

	char *path = "/home/kazuy/ws/usb";
	int ret = 0;

	ret = setHome(&cwd, path);
	if(ret != 0)
		return ret;


	usb_t f;
	if(!usbInit(&f))
		exit(1);

	idxPool_t pool;
	idxPoolInit(&pool, f.capacity);

	
	int check = makedir(f.cwd.path, 0777);
	if(check == EFAULT)
			exit(EXIT_FAILURE);
	
	openDir(&cwd, " ", &f,ONEWEEK * 6, &pool);
	
	//reset
	setHome(&cwd, path);

	startBackUp(&f,  path, &cwd, "/mnt/usb/copied", &pool);

	freedata(&f, &pool);
	
	stackFree(&pool.idxAvailable);
	free(cwd.path);
	cwd.path = NULL;

	return 0;
}
