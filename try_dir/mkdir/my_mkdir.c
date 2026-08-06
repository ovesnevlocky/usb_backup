#include "my_mkdir.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h> 
#include <stdint.h>


int makedir(const char *dir, const uint32_t mode)
{
	int check = mkdir(dir , mode);
	if(!check)
	{
		printf("Directory created at %s\n", dir);
		return check;
	}
	else
	{
		//perror("mkdir");
		return errno;
	}
	return check;
}