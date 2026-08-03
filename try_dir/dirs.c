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


#define ONEMIN 60

#define ONEDAY 24*60*ONEMIN

#define ONEWEEK 7*ONEDAY

typedef struct
{
	char *pathOriginal;
	char *pathUsb;
	time_t modified_at;

}file_t;

typedef struct 
{
	file_t *files;
	size_t count;
	char cwdUsb[PATH_MAX];

}files;


bool isNull(const void *a);

void printStat(const char *path);

void concat(char *dst, const char *dir_to);

void cleanDirTo(char *dst,const  char *path);

time_t getStat(char *path, int period);

static bool isInSameDir(const char *cwdUsb,const char * dir_to);

char *cpyPath(const char *path)
{
	char *ret = malloc(sizeof(char) * (strlen(path) + 1));
	memcpy(ret, path, strlen(path) + 1);

	return ret;	
}


	//change back to the string before concating//
void cleanDirTo(char *cwd,const char *dir_to)
{

	size_t lenDirTo = isNull(dir_to) ? 0 : strlen(dir_to);
	size_t idx = strlen(cwd) - 1 ;

	while(idx > 0 && cwd[idx] !='/')
	{
		cwd[idx] = '\0';
		idx--;
	}
	cwd[idx] = '\0';
}

static inline bool isParentDir(const char *d_name)
{
	return strcmp(d_name, "..") == 0;
}	

static inline bool isCurrDir(const char *d_name)
{
	return strcmp(d_name, ".") == 0;
}	

static inline bool isHidden(const char *d_name)
{
	return d_name[0] == '.';

}

static inline bool isGit(const char *d_name)
{
	return strcmp(d_name, ".git") == 0;
}	

//copy a file, returns path in Usb if success, otherwise NULL
char * copyFile(const char *cwd, const char *fname, char *pathUsb)
{
	FILE *fp = fopen(cwd, "rb");
	if(!fp)
	{
		perror(cwd);
		return NULL;
	}
	
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

		memset(buff, 0, sizeof(buff));
	}

	char *ret = cpyPath(saveTo);

	fclose(fp_out);
	fclose(fp);
	return ret;
}


bool makedir(const char *dir)
{
	int check = mkdir(dir ,0777);
	if(!check)
	{
		printf("Directory created at %s\n", dir);
		return check;
	}
	else
		perror("mkdir");
	
}

void openDir(char *cwd, char *dir_to, files *list, const int period)
{

	concat(cwd, dir_to);

	time_t modified_at = getStat(cwd, period);
	if(modified_at)
	{
		concat(list->cwdUsb, dir_to);
		makedir(list->cwdUsb);
	}


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
			if(isParentDir(dp->d_name) || isCurrDir(dp->d_name) 
			||  isHidden(dp->d_name))
			{
				continue;	
			}
			
			openDir (cwd, dp->d_name, list, period);
		}
		else if(dp ->d_type == DT_REG)
		{
			if(isHidden(dp->d_name))
				continue;

			concat(cwd, dp->d_name);
			time_t Fmodified_at = getStat(cwd, period);

			if(Fmodified_at)
			{
				list->files[list->count].modified_at = modified_at;
				list->files[list->count].pathOriginal = cpyPath(cwd);
				char * ret = copyFile(cwd, dp->d_name, list->cwdUsb);	
				if(isNull(ret))
				{
					continue;	
				}

				list->files[list->count].pathUsb = ret;
				list->count += 1;
			}
			cleanDirTo(cwd, dp->d_name);
		}
	}while(dirp);

	closedir(dirp);	
	cleanDirTo(cwd, dir_to);

	//leave from dirctory in usb when original cwd leaves//
	if(isInSameDir(list->cwdUsb, dir_to))
	{
		printf("leaving dir %s\n", list->cwdUsb);	
		cleanDirTo(list->cwdUsb, dir_to);
	}
	return;
}


static bool isInSameDir(const char *cwdUsb,const char * dir_to)
{
	if(isNull(cwdUsb) || isNull(dir_to))
	{
		fprintf(stderr, "%s is points to NULL\n", 
				cwdUsb == NULL ? cwdUsb : dir_to);
	}

	size_t len = strlen(dir_to);
	size_t lenU = strlen(cwdUsb);

	printf("%s, %s\n", cwdUsb + lenU - len, dir_to);
	return strncmp(cwdUsb + lenU - len, dir_to, len ) == 0;

	
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
	{
		//dst[strlen(dst)] = '/';
		return;
	}
		

	dst[strlen(dst)] = '/';
	memcpy(dst + strlen(dst), dir_to, strlen(dir_to) + 1);

	return;
}


void setHome(char *dst, char *path)
{
	memcpy(dst, path, strlen(path) + 1); 
}

//returns time if a given lastModified is within given period otherwise 0
time_t isModifiedWithinPeriod(time_t lastModified, int period)
{
	time_t now;
	now = time(NULL);
	
	return lastModified + period  >= now ? lastModified : 0;

}


time_t getStat(char *path, int period)
{
	struct stat sb;
	
    	if (lstat(path, &sb) == -1)	
       	{
        	perror("lstat");
		printf("%s: \n", path);
		
        	return false;
    	}

	 return isModifiedWithinPeriod(sb.st_mtime, period);

}


void freedata(files *f)
{
	for(int i = 0; i < f->count; i++)
	{
		free(f->files[i].pathOriginal);
		free(f->files[i].pathUsb);

		f->files[i].pathOriginal = NULL;
		f->files[i].pathUsb = NULL;
	}

	free(f->files);
	f->files = NULL;
}

int main()
{
	char cwd[PATH_MAX] = {0};
	//getcwd(cwd, sizeof(cwd));
	puts(cwd);
	char *path = "/home/kazuy/ws/usb";
	setHome(cwd, path);	
	//getStat(cwd);
	files f = {0};
	f.files = malloc(sizeof(file_t) * 100);

	setHome(f.cwdUsb, "/mnt/usb/copied");

	int check = mkdir(f.cwdUsb,0777);
	if(!check)
		printf("Directory created at %s\n", f.cwdUsb);
	else
		perror("mkdir");
	
	openDir(cwd, " ", &f, ONEDAY);

	freedata(&f);

	return 0;
}

void startBackUp(files *f, char *home)
{
	int period = ONEMIN;

	while(true)
	{
		for(int i = 0; i < f->count; i++)
		{
			file_t curr = f->files[i];
			time_t a;
		
		}
	}


}

void printStat(const char *path)
{
	struct stat sb;


        if (lstat(path, &sb) == -1) {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        printf("ID of containing device:  [%x,%x]\n",
               major(sb.st_dev),
               minor(sb.st_dev));

        printf("File type:                ");

        switch (sb.st_mode & S_IFMT) {
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
