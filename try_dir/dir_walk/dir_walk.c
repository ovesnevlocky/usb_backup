#include "dir_walk.h"
#include <time.h>
#include <dirent.h>
#include <errno.h>

#include <stdlib.h>
#include "../usb/usb.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/limits.h>
#include "../mem_pool/mem_pool.h"
#include <sys/types.h>

#include "../path_utils/path_utils.h"
#include"../mkdir/my_mkdir.h"
#include "../backup/backup.h"

#include <string.h>

static bool  inicializeNewList(usb_t *list)
{

	size_t oldC = list->capacity;
	list->capacity *= 2;
	void *tmp = myRealloc(list->files, list->capacity * sizeof(file_t));
	if(tmp)
	{
		list->files = tmp;
		while(oldC < list->capacity)
		{
			list->files[oldC].pathOriginal = NULL;
			list->files[oldC].pathUsb = NULL;
			list->files[oldC].idx = -1;
			oldC++;
		}	
	}
	else
	{
		perror("realloc");
		return false;
	}

	return true;

}

void openDir(path_t *cwd, char *dir_to, usb_t *list, const uint32_t period, idxPool_t *p)
{
	//if usb has no more space
	if(list->byteWritten >= list->limit)
		return;

	concat(cwd->path, dir_to, cwd->size);


	time_t modified_at = getStat(cwd->path, period);

	int ret = 0;
	if(modified_at)
	{

		ret = concat(list->cwd.path, dir_to, list->cwd.size);
		if(ret == BUFF_OVERFLOW)
		{
			enlargePath(cwd);
		}

		errno = 0;
		int ret = makedir(list->cwd.path, 0777);
		if(ret != 0)
		{
			perror("mkdir");
		}
	
	}

	DIR *dirp = opendir(cwd->path);

	if(dirp == NULL)
	{
		cleanDirTo(cwd->path);
		perror(cwd->path);
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

			openDir (cwd, dp->d_name, list, period, p);
		}
		else if(dp ->d_type == DT_REG)
		{
			if(isHidden(dp->d_name))
				continue;

			ret = concat(cwd->path, dp->d_name, cwd->size);
			if(ret == BUFF_OVERFLOW)
				enlargePath(cwd);

			time_t Fmodified_at = getStat(cwd->path, period);
			char saveTo[PATH_MAX] = {0};

			memcpy(saveTo, list->cwd.path, strlen(list->cwd.path) + 1);
			ret = concat(saveTo, dp->d_name, sizeof(saveTo));
			if(ret == BUFF_OVERFLOW)
				enlargePath(cwd);

			//dont wanto make another copy if already copied,
			//or if not modifed  within given period
			if(isAlreadyCopied(saveTo) == true||
				Fmodified_at == 0)
			{
				cleanDirTo(cwd->path);
				continue;
			}

			fprintf(stderr, "%s was newly modified\n", cwd->path);
			if(list->count >= list->capacity)
			{
				if(!inicializeNewList(list))
					fprintf(stderr, "failto realloc\n");	

			}

			int64_t byteWritten = copyFile(cwd->path, saveTo, list);
			if(byteWritten < 0 )
			{
				cleanDirTo(cwd->path);
				fprintf(stderr, "error in CopyFile %i\n", (int) byteWritten);
				continue;
			}
			//make sure theres valid idx before popping	
			if(isStackEmpty(&p->idxAvailable))
			{
				int ret = enlargePool(p);
				if(ret != 0)
					fprintf(stderr, "enlargePool retuned %i\n", ret);
			}
			//pop an available idx from stack 
			int idx = stackPop(&p->idxAvailable);
			//mark as being used
			if(p->idxInUse[idx] == false)
				p->idxInUse[idx] = true;
			else
				fprintf(stderr, "this idx:%i is in use...\n", idx);
			
			list->files[idx].pathUsb = cpyPath(saveTo);
			list->files[idx].modified_at = Fmodified_at;
			list->files[idx].pathOriginal = cpyPath(cwd->path);
			list->files[idx].size = (uint64_t) byteWritten;
			//assign the popped idx
			list->files[idx].idx = idx;
			list->byteWritten += (uint64_t) byteWritten;
			list->count += 1;
			p->count += 1;

			cleanDirTo(cwd->path);
		}

	}while(dirp && isUsbMounted());

	closedir(dirp);	
	cleanDirTo(cwd->path);

	//leave from dirctory in usb when original cwd leaves//
	if(isInSameDir(list->cwd.path, dir_to))
	{
		printf("leaving dir %s\n", list->cwd.path);	
		cleanDirTo(list->cwd.path);
	}
	return;
}

