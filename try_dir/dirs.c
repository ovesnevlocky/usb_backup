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

#include <time.h>

#include <sys/sysmacros.h>

#include <stdint.h>

#define ONEWEEK 7*24*60*60

typedef struct 
{
	char *paths[4096];
	char *pathUsb[4096];
	time_t modified_at;
}file_t;	

bool isNull(const void *a);


void concat(char *dst, const char *dir_to);

void cleanDirTo(char *dst,const  char *path);

time_t getStat(char *path);

char *cpyPath(const char *path)
{
	char *ret = malloc(sizeof(char) * (strlen(path) + 1));
	memcpy(ret, path, strlen(path) + 1);

	return ret;	
}


void cleanDirTo(char *cwd,const char *dir_to)
{

	size_t lenDirTo = isNull(dir_to) ? 0 : strlen(dir_to);
	
	//plus one for '/' char
	size_t offset = lenDirTo + 1;
	size_t start = strlen(cwd) - offset;
	//change back to the string before concating//
	memset(cwd + start , 0,  offset);

}

static inline bool isParentDir(char *d_name)
{
	return strcmp(d_name, "..") == 0;
}	

static inline bool isCurrDir(char *d_name)
{
	return strcmp(d_name, ".") == 0;
}	

static inline bool isHidden(char *d_name)
{
	return d_name[0] == '.';

}

size_t numDir = 0;
size_t numFile = 0;


char * copyFile(const char *cwd, const char *fname)
{
	FILE *fp = fopen(cwd, "rb");
	if(!fp)
	{
		perror(cwd);
		return NULL;
	}
	
	char *pathUsb = "/mnt/usb/copied";

	char saveTo[PATH_MAX] = {0};
	memcpy(saveTo, pathUsb, strlen(pathUsb));
	concat(saveTo, fname);


	FILE *fp_out = fopen(saveTo, "wb");
	if(!fp_out)
	{
		perror(cwd);
		return false;
	}

	char buff[4096] = {0};

	fprintf(fp_out, "original at: %s\n", cwd);
	size_t byteWritten, byteRead;	
	while(byteRead = fread(buff, 1, sizeof(buff), fp))
	{
		if(byteRead <= 0)
			break;
		byteWritten = fwrite(buff, 1, byteRead, fp_out);
		if(byteWritten != byteRead)
		{
			printf("written :%lu, read: %lu\n",  byteWritten, byteRead);
			perror("fwrite");
			fclose(fp_out);
			fclose(fp);
			return NULL;
		}	
	}

	char *ret = cpyPath(saveTo);

	fclose(fp_out);
	fclose(fp);
	return ret;
}


void openDir(char *cwd, char *dir_to, file_t *list, int *count)
{

	concat(cwd, dir_to);
	//fprintf(stderr, "%s\n", cwd);
	DIR *dirp = opendir(cwd);
	if(dirp == NULL)
	{
		cleanDirTo(cwd, dir_to);
		perror(cwd);
		return;
	}
        struct dirent *dp; 
	time_t modified_at;	
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
			if(isParentDir(dp->d_name) || isCurrDir(dp->d_name) || isHidden(dp->d_name))
			{
				continue;	
			}

			//fprintf(stdout, "-----open dir: %s -----\n", dp->d_name);
			numDir++;	
			openDir (cwd, dp->d_name, list, count);
		}
		else if(dp ->d_type == DT_REG)
		{
			concat(cwd, dp->d_name);
			modified_at = getStat(cwd);
			if(modified_at)
			{
				//printf("%s was modified within a week\n", cwd);
				list[*count].paths[*count] = cpyPath(cwd);
				list[*count].modified_at = modified_at;
				char * ret = copyFile(cwd, dp->d_name);	
				if(isNull(ret))
				{
					continue;
				}
				list[*count].pathUsb[*count] = ret;
				printf("path copied: %s\n path src %s\n", ret, list[*count].paths[*count]);
				*count += 1;

			}
			cleanDirTo(cwd, dp->d_name);
			numFile++;
			//fprintf(stdout, "file: %s\n", dp->d_name);
		}
	}while(dirp);
	closedir(dirp);	
	cleanDirTo(cwd, dir_to);
//	fprintf(stdout, "-----close dir: %s -----\n\n", strcmp(dir_to, " ") == 0 ? cwd : dir_to);
	return;
}

bool isNull(const void *a)
{
	return a == NULL;
}

void concat(char *dst,const char *dir_to)
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


void setHome(char *cwd)
{
	char *path = "/home/kazuy/ws/usb/try_dir/test1";
	int i = 0;
	for(; path[i] != '\0'; i++)
		cwd[i] = path[i];
	cwd[i] = '\0';
}

//returns time if a given lastModified is within a week otherwise 0
time_t isModifiedWithinWeek(unsigned long lastModified)
{
	time_t now;
	now = time(NULL);
	
	return lastModified + ONEWEEK  >= now ? lastModified : 0;

}

time_t getStat(char *path)
{
	struct stat sb;
	
    	if (lstat(path, &sb) == -1)	
       	{
		printf("%s: ", path);
        	perror("lstat");
        	return false;
    	}

	 return isModifiedWithinWeek(sb.st_mtime);

}



int main()
{
	char cwd[PATH_MAX] = {0};
	getcwd(cwd, sizeof(cwd));
	puts(cwd);	

	setHome(cwd);	
	//getStat(cwd);
	file_t *list = malloc(sizeof(file_t) * 100);	
	int count = 0;	
	openDir(cwd, " ", list, &count);
	printf("Dir: %lu, file: %lu\n", numDir, numFile);

	for(int i = 0; i < count; i++)
	{
		free(list[i].paths[i]);
		free(list[i].pathUsb[i]);
	}


	free(list);
}
