#include "../path_utils.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{
	printf("hello\n");

	char *p = "/mnt/usb/copied/foobar";

	if(isInSameDir("/mnt/usb/copied/foobarr", "foobar"))
		printf(" good");

	if(isInSameDir(NULL, NULL))
		printf("null check faile\n");

	if(isInSameDir(p, ""))
		fprintf(stderr, "aaa");

	if(isInSameDir("", ""))
		fprintf(stderr, "not okay\n");

	if(!isInSameDir("/mnt", "mnt"))
		fprintf(stderr, "not okej\n");


	char buff[8] = {0};
	strcpy(buff, "/abc");
	concat(buff, "abc");
	printf("%s\n", buff);	
	return 0;

}
