#include <stdio.h>

#include <stdlib.h>

#include <string.h>

void printS(char *buff)
{
	printf("printS: %lu\n", sizeof(buff));

}

void printSp(size_t n, char buff[n])
{
	printf("printSp: %lu\n", sizeof(buff));

}


int main()
{

	char buff[4096] = {0};

//	printf("main: %lu\n", sizeof(buff));

//	printS(buff);
	char aa[3] = {0};
	printS(aa);

//	printSp(sizeof(buff), buff);
}


