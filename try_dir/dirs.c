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
#include <stdint.h>
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
	int idx;
	time_t modified_at;
}file_t;

typedef struct 
{
	file_t *files;
	size_t count;
	uint16_t capacity;
	uint32_t byteWritten;
	uint32_t limit;
	char cwdUsb[PATH_MAX];
}usb_t;


static inline bool isNull(const void *a);

void printStat(const char *path);

void concat(char *dst, const char *dir_to);

void cleanDirTo(char *dst,const  char *path);

time_t getStat(char *path, int period);

bool isInSameDir(const char *cwdUsb,const char * dir_to);

void openDir(char *cwd, char *dir_to, usb_t *list, const int period);

char *cpyPath(const char *path);

static inline bool isParentDir(const char *d_name);

static inline bool isCurrDir(const char *d_name);

static inline bool isHidden(const char *d_name);

int makedir(const char *dir);

void concat(char *dst,const char *dir_to);
void setHome(char *dst, char *path);
time_t isModifiedWithinPeriod(time_t lastModified, int period);
time_t getStat(char *path, int period);
void freedata(usb_t *f);
void startBackUp(usb_t *f, char *home);

void *myRealloc(void *old, size_t newSize);

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

//copy a file, returns  bytes written  if success, otherwise 0
uint32_t copyFile(const char *cwd,const  char *saveTo)
{
	if(isNull(cwd) || isNull(saveTo))
	{
		fprintf(stderr, "%s is points to NULL at copyFile\n", 
				cwd == NULL ? "cwd" : "saveTo" );
		return 0;
	}


	FILE *fp = fopen(cwd, "rb");
	if(!fp)
	{
		perror(cwd);
		return 0;
	}

	//if file exits but modified time is newer, then make another copy//
	//if getStat returns error no such files (ENOENT), means the file not yet copied
	//then just make a new copy
	//if theres, if getStat returns positive value, then compare
	//these values and if the value is bigger, make a copy
	//if these values are the same, then not make copy...
	
		
	FILE *fp_out = fopen(saveTo, "wb");
	if(!fp_out)
	{
		perror(cwd);
		return 0;
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
			return 0;
		}

		memset(buff, 0, sizeof(buff));
	}

	fclose(fp_out);
	fclose(fp);
	return byteWritten;
}


int makedir(const char *dir)
{
	int check = mkdir(dir ,0777);
	if(!check)
	{
		printf("Directory created at %s\n", dir);
		return check;
	}
	else
	{
		perror("mkdir");
		return errno;
	}

	return check;
	
}

void openDir(char *cwd, char *dir_to, usb_t *list, const int period)
{

	concat(cwd, dir_to);

	time_t modified_at = getStat(cwd, period);
	if(modified_at)
	{
		concat(list->cwdUsb, dir_to);
		int ret = makedir(list->cwdUsb);
	}

	DIR *dirp = opendir(cwd);

	if(dirp == NULL)
	{
		cleanDirTo(cwd, dir_to);
		perror(cwd);
		return;
	}

	do	
	{
		errno = 0;
		struct dirent *dp = readdir(dirp);
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
				if(list->count >= list->capacity)
				{
					list->capacity *= 2;
					myRealloc(list->files, list->capacity * sizeof(file_t));

				}
				fprintf(stderr, "%s was newly modified\n", cwd);
				char saveTo[PATH_MAX] = {0};
				strcpy(saveTo, list->cwdUsb);
				concat(saveTo, dp->d_name);
				uint32_t byteWritten = copyFile(cwd, saveTo);
				if(byteWritten)
				{
					list->files[list->count].pathUsb = cpyPath(saveTo);
					list->files[list->count].modified_at = modified_at;
					list->files[list->count].pathOriginal = cpyPath(cwd);
					list->files[list->count].idx = list->count;
					list->byteWritten += byteWritten;
					list->count += 1;
				}
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


bool isInSameDir(const char *cwdUsb,const char * dir_to)
{
	if(isNull(cwdUsb) || isNull(dir_to))
	{
		fprintf(stderr, "%s is points to NULL at isInSameDir\n", 
				cwdUsb == NULL ? "cwdUsb" : "dir_to" );
		return false;
	}

	size_t len = strlen(dir_to);
	size_t lenU = strlen(cwdUsb);

	return strncmp(cwdUsb + lenU - len, dir_to, len ) == 0;
	
}

bool isNull(const void *a)
{
	return a == NULL;
}

void concat(char *dst,const char *dir_to)
{
	if(isNull(dst) || isNull(dir_to))
	{
		fprintf(stderr, "%s is points to NULL in concat\n", 
				dst == NULL ? "dst" : "dir_to" );
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
        	return errno;
    	}

	 return isModifiedWithinPeriod(sb.st_mtime, period);

}

void freedata(usb_t *f)
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

void *myRealloc(void *old, size_t newSize)
{
	void *new = realloc(old, newSize);
	if(!new)
	{
		perror("realloc");
		return NULL;		
	}

	return new;
}

int main()
{
	char cwd[PATH_MAX] = {0};

	//getcwd(cwd, sizeof(cwd));
	puts(cwd);
	char *path = "/home/kazuy/ws/usb";
	setHome(cwd, path);	
	//getStat(cwd);
	usb_t f = {0};
	f.capacity = 100;
	f.count = 0;
	f.files = malloc(sizeof(file_t) * f.capacity);

	setHome(f.cwdUsb, "/mnt/usb/copied");

	int check = mkdir(f.cwdUsb,0777);
	if(!check)
		printf("Directory created at %s\n", f.cwdUsb);
	else
		perror("mkdir");
	
	openDir(cwd, " ", &f,ONEDAY* 3);

	fprintf(stderr, "%s\n", path);
	startBackUp(&f, path);

	freedata(&f);

	return 0;
}

void checkFiles(usb_t *f, const int period)
{

	int count = f->count;	
	fprintf(stderr, "----------------------------\n");	
	int i = 0;

	while(count > 0)	
	{

		if(isNull(f->files[i].pathOriginal) || isNull(f->files[i].pathUsb))
		{
			i++;
			continue;
		}

		time_t modified_at = getStat(f->files[i].pathOriginal, period);
		fprintf(stderr, "checking %s\n", f->files[i].pathOriginal);
		//removed, moved, .....
		if(errno == ENOENT)
		{
			fprintf(stderr, "%i: %s points to NULL\n"
					,f->files[i].idx,  f->files[i].pathOriginal);
			free(f->files[i].pathOriginal);
			free(f->files[i].pathUsb);
			f->files[i].pathOriginal = NULL;
			f->files[i].pathUsb = NULL;
			f->count -= 1;
			errno = 0;
		}
		else if(modified_at)
		{
			//make new copy
			copyFile(f->files[i].pathOriginal, f->files[i].pathUsb);
			f->files[i].modified_at = modified_at;
			fprintf(stderr, "%s was changed make another copy\n", f->files[i].pathOriginal);
		}
		i++;
		count--;
	}

}

void startBackUp(usb_t *f, char *home)
{
	int period = ONEMIN/6;
	errno = 0;
	int freeListHead = f->count;
	while(true)
	{
		checkFiles(f, period);

		sleep(period);
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

