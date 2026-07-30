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

bool isNull(void *a);


void concat(char *dst, const char *dir_to);

void cleanDirTo(char *dst,const  char *path);

bool getStat(char *path);

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


bool copyFile(const char *cwd, const char *fname)
{
	FILE *fp = fopen(cwd, "rb");
	if(!fp)
	{
		perror(cwd);
		return false;
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
			return false;
		}	
	}

	fclose(fp_out);
	fclose(fp);
	return true;
}


void openDir(char *cwd, char *dir_to, char **list, int *count)
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
			if(getStat(cwd))
			{
				//printf("%s was modified within a week\n", cwd);
				list[*count] = cpyPath(cwd);
				copyFile(cwd, dp->d_name);	
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

bool isNull(void *a)
{
	return a == NULL;
}

void concat(char *dst,const  char *dir_to)
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
	char *path = "/home/kazuy/ws";
	int i = 0;
	for(; path[i] != '\0'; i++)
		cwd[i] = path[i];
	cwd[i] = '\0';
}


bool isModifiedWithinWeek(unsigned long lastModified)
{
	time_t now;
	now = time(NULL);
	
	return lastModified + ONEWEEK  >= now;

}


bool getStat(char *path)
{
	struct stat sb;
	
    	if (lstat(path, &sb) == -1)	
       	{
		printf("%s: ", path);
        	perror("lstat");
        	return false;
    	}

	/*
	printf("ID of containing device:  [%x,%x]\n",
               major(sb.st_dev),
               minor(sb.st_dev));

        printf("File type:                ");

        switch (sb.st_mode & S_IFMT) 
	{
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
         }

         printf("I-node number:            %ju\n", (uintmax_t) sb.st_ino);

         printf("Mode:                     %jo (octal)\n",
                (uintmax_t) sb.st_mode);

         printf("Link count:               %ju\n", (uintmax_t) sb.st_nlink);
         printf("Ownership:                UID=%ju   GID=%ju\n",
                  (uintmax_t) sb.st_uid, (uintmax_t) sb.st_gid);

         printf("Preferred I/O block size: %jd bytes\n",
                 (intmax_t) sb.st_blksize);
         printf("File size:                %jd bytes\n",
                  (intmax_t) sb.st_size);
         printf("Blocks allocated:         %jd\n",
                 (intmax_t) sb.st_blocks);

         printf("Last status change:       %s", ctime(&sb.st_ctime));
         printf("Last file access:         %s", ctime(&sb.st_atime));
         printf("Last file modification:   %s", ctime(&sb.st_mtime));

	 
         printf("Last file modification:   %lu\n", sb.st_mtime);
	 */

	 return isModifiedWithinWeek(sb.st_mtime);

}


int main()
{
	char cwd[PATH_MAX] = {0};
	getcwd(cwd, sizeof(cwd));
	puts(cwd);	

	setHome(cwd);	
	//getStat(cwd);
	char **list = malloc(sizeof(char *) * 100);	
	int count = 0;	
	openDir(cwd, " ", list, &count);
	printf("Dir: %lu, file: %lu\n", numDir, numFile);

	for(int i = 0; i < count; i++)
		free(list[i]);

	free(list);
}
