
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


void concat(char *dst, char *dir_to);

bool isParentDir(char *d_name)
{
	return strcmp(d_name, "..") == 0;
}	

bool isCurrDir(char *d_name)
{
	return strcmp(d_name, ".") == 0;
}	


void openDir(char *cwd, char *dir_to)
{
	concat(cwd, dir_to);
	DIR *dirp = opendir(cwd);
        struct dirent *dp; 
	
	do	
	{
		errno = 0;
		dp = readdir(dirp);
		if(dp == NULL)
		{
			if(errno !=  0)
				perror("readdir");
			break;	
		}			
	
		if(dp ->d_type == DT_DIR)
		{	
			if(isParentDir(dp->d_name) || isCurrDir(dp->d_name))
			{
				continue;	
			}

			fprintf(stderr, "open dir: %s\n", dp->d_name);
