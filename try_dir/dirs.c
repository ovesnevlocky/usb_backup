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

bool isNull(void *a);


void concat(char *dst, char *dir_to);

void cleanDirTo(char *dst, char *path);

	
void cleanDirTo(char *cwd, char *dir_to)
{

	size_t lenDirTo = isNull(dir_to) ? 0 : strlen(dir_to);
	
	size_t offset = lenDirTo + 1;
	size_t start = strlen(cwd) - offset;
	//change back to the string before concating//
	memset(cwd + start , 0,  offset);

}
bool isParentDir(char *d_name)
{
	return strcmp(d_name, "..") == 0;
}	

bool isCurrDir(char *d_name)
{
	return strcmp(d_name, ".") == 0;
}	

size_t numDir = 0;
size_t numFile = 0;
void openDir(char *cwd, char *dir_to)
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
			if(isParentDir(dp->d_name) || isCurrDir(dp->d_name))
			{
				continue;	
			}

			//fprintf(stdout, "-----open dir: %s -----\n", dp->d_name);
			numDir++;	
			openDir (cwd, dp->d_name);
		}
		else if(dp ->d_type == DT_REG)
		{
			numFile++;
			//fprintf(stdout, "file: %s\n", dp->d_name);
		}
	}while(dirp);
	//plus one for '/' char
	closedir(dirp);	
	cleanDirTo(cwd, dir_to);
//	fprintf(stdout, "-----close dir: %s -----\n\n", strcmp(dir_to, " ") == 0 ? cwd : dir_to);
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


void setHome(char *cwd)
{
	char *path = "/home/kazuy";
	int i = 0;
	for(; path[i] != '\0'; i++)
		cwd[i] = path[i];
	cwd[i] = '\0';
}

void getStat(char *path)
{
	struct stat sb;
	
    	if (lstat(path, &sb) == -1)	
       	{
        	perror("lstat");
        	return;
    	}

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



}


int main()
{
	char cwd[PATH_MAX] = {0};
	getcwd(cwd, sizeof(cwd));
	puts(cwd);	
	getStat(cwd);

//	setHome(cwd);	

	openDir(cwd, " " );
	printf("Dir: %lu, file: %lu\n", numDir, numFile);
	
}
