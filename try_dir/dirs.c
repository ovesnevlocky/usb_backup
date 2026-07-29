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

bool isNull(void *a);


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
	//fprintf(stderr, "%s\n", cwd);
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

			fprintf(stdout, "-----open dir: %s -----\n", dp->d_name);
			
			openDir (cwd, dp->d_name);
		}
		else if(dp ->d_type == DT_REG)
		{
			fprintf(stdout, "file: %s\n", dp->d_name);
		}
	}while(dirp);
	//plus one for '/' char
	fprintf(stdout, "-----close dir: %s -----\n\n", strcmp(dir_to, " ") == 0 ? "try_dir" : dir_to);

	closedir(dirp);	
	size_t lenDirTo = isNull(dir_to) ? 0 : strlen(dir_to);
	
	size_t offset = lenDirTo + 1;
	size_t start = strlen(cwd) - offset;
	//change back to the string before concating//
	memset(cwd + start , 0,  offset);
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
	//getcwd(cwd, sizeof(cwd));
	char *path = "/home/kazuy";
	int i = 0;
	for(; path[i] != '\0'; i++)
		cwd[i] = path[i];


	cwd[i] = '\0';
	puts(cwd);
	openDir(cwd, " " );
	
}
