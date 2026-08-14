#include "../path_utils.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


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
	char buff[8] = {0};

	char *home = "/home/ka";
	int ret = setHome(buff, home);
	assert(ret != 0);

	setHome(NULL, home);
	assert(ret != 0);

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
