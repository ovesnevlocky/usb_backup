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

void setHomeTest()
{
	char *home = "/home/ka";


}

void concatTest()
{
	char buff[8] = {0};
	strcpy(buff, "/home");
	int ret = concat(buff, "a");

	if(ret != 0)
		printError(ret);
	else 
		fprintf(stderr , "%s\n", buff);


	char buff1[PATH_MAX] = {0};
	buff1[0] = '/';

	//case upper bound ok
	for(int i = 1; i < 4093; i++)
		buff1[i] = 'a';

	printf("%lu\n", strlen(buff1));

	ret = concat(buff1, "a");
	if(ret != 0)
		printError(ret);	
	else
		printf("%s\n %lu\n", buff1, strlen(buff1));

	char buff2[PATH_MAX] = {0};
	buff2[0] = '/';

	for(int i = 1; i < 4093; i++)
		buff2[i] = 'a';
	
	ret = concat(buff2, "");
	if(ret != 0)
		printError(ret);
	else 
		printf("%s\n", buff2);	

}

int main()
{
	char *p = "/mnt/usb/copied/foobar";


	char buff[8] = {0};
	strcpy(buff, "/abc");
	concat(buff, "abc");
	printf("%s\n", buff);	

	setHomeTest();

	concatTest();

	return 0;

}
