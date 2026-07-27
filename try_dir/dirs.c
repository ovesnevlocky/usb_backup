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
			openDir (cwd, dp->d_name);
		}
		else if(dp ->d_type == DT_REG)
		{
			fprintf(stderr, "file: %s\n", dp->d_name);
		}
	}while(dirp);
	//plus one for '/' char
	closedir(dirp);	
	size_t lenDirTo = dir_to == NULL ? 0 :  strlen(dir_to);
	printf("len cwd: %lu, len dir_to: %lu, %s\n", strlen(cwd), lenDirTo, dir_to);
	cwd[strlen(cwd) - ( lenDirTo + 1 ) ] = '\0';
	return;
}

bool isNull(void *a)
{
	return a == NULL;
}

void concat(char *dst, char *dir_to)
{
	if(isNull(dst))
	{
		fprintf(stderr, "dst points to null\n");
		return;
	}

	if(isNull(dir_to))
	{
		fprintf(stderr, "dir_to points to null\n");
		return;
	}

	//the first case
	if(strcmp(dir_to, " ") == 0)
			return;

	int i = 0;
	int dstIdx = strlen(dst);
	dst[dstIdx++] = '/';

	for(; dir_to[i] != '\0'; i++)
	{
		dst[dstIdx++] = dir_to[i];
	}
	dst[dstIdx] = '\0';
	
	return;
}

int main()
{
	char cwd[PATH_MAX] = {0};
	getcwd(cwd, sizeof(cwd));
	char *first = " ";	
	openDir(cwd, first);
	
}
