#include "../path_utils.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <linux/limits.h>


void printError(int err)
{
	switch(err)
	{
		case PATH_NULL:
			fprintf(stderr, "path points to null\n");
			break;
		case BUFF_OVERFLOW: 
				fprintf(stderr, "buffer overflow\n");
				break;
		case PATH_TOO_LONG:
				fprintf(stderr ,"path length  is over path_max\n");
				break;
		case PATH_NO_SLASH_AT_START:
				fprintf(stderr, "absolute path needs / at the start\n");
				break;
		default:
				fprintf(stderr , "unkown error %i\n", err);
				break;
	
	}


}

int main()
{
	char p[4096] = "/mnt/usb/copied/aaaaaaaa";

	char *dir = "aaaaaaaa";

	printf("%lu\n", strlen(dir));

	int ret = cleanDirTo(p,strlen(dir));

	printf("%s\n", p);

	return 0;

}
