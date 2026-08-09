#include "file_stat.h"

#include <time.h>
#include <stdint.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <stdlib.h>
#include <time.h>
#include <linux/stat.h>
#include <stdio.h>


//returns time if a given lastModified is within given period otherwise 0
time_t isModifiedWithinPeriod(time_t lastModified, const uint32_t period)
{
	time_t now;
	now = time(NULL);
	
	return lastModified + (time_t) period  >= now ? lastModified : 0;
}


time_t getStat(char *path, const uint32_t period)
{
	struct stat sb;
	errno = 0;	
    	if (lstat(path, &sb) == -1)	
       	{
        	perror("lstat");
		//return 0 to indicate error
		return 0;
    	}

	 return isModifiedWithinPeriod(sb.st_mtime, period);

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
