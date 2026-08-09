#include "my_mkdir.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h> 
#include <stdint.h>


int makedir(const char *dir, const uint32_t mode)
{
	errno = 0;
	int check = mkdir(dir , mode);
	//if already exists, its ok
	if(check == 0)
	{
		return check;
	}

	return check;
}
