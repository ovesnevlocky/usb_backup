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

void openDir(char *cwd, char *dir_to, usb_t *list, const uint32_t period, idxPool_t *p)
{
	concat(cwd, dir_to);

	time_t modified_at = getStat(cwd, period);
	if(modified_at)
	{
		concat(list->cwdUsb, dir_to);
		int ret = makedir(list->cwdUsb, 0777);
		if(!ret)
			perror("mkdir");
	
	}

	DIR *dirp = opendir(cwd);

	if(dirp == NULL)
	{
		cleanDirTo(cwd);
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

			openDir (cwd, dp->d_name, list, period, p);
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
				memcpy(saveTo, list->cwdUsb, strlen(list->cwdUsb) + 1);
				concat(saveTo, dp->d_name);

				//dont want to make another copy if already copied
				if(isAlreadyCopied(saveTo) == true)
				{
					//fprintf(stderr, "%s is already in usb\n", saveTo);
					
					//make sure to clean the cwd
					cleanDirTo(cwd);
					continue;
				}

				uint64_t byteWritten = copyFile(cwd, saveTo, list);
				if(byteWritten)
				{
					//pop an available idx from stack 
					int idx = stackPop(&p->idxAvailable);

					//mark as being used
					if(p->idxInUse[idx] == false)
						p->idxInUse[idx] = true;
					else
						fprintf(stderr, "this idx:%i is in use...\n", idx);
					
					list->files[idx].pathUsb = cpyPath(saveTo);
					list->files[idx].modified_at = modified_at;
					list->files[idx].pathOriginal = cpyPath(cwd);
					list->files[idx].size = byteWritten;
					//assign the popped idx
					list->files[idx].idx = idx;
					list->byteWritten += byteWritten;
					list->count += 1;
					p->count += 1;

				}
			}
			cleanDirTo(cwd);
		}
	}while(dirp);

	closedir(dirp);	
	cleanDirTo(cwd);

	//leave from dirctory in usb when original cwd leaves//
	if(isInSameDir(list->cwdUsb, dir_to))
	{
		printf("leaving dir %s\n", list->cwdUsb);	
		cleanDirTo(list->cwdUsb);
	}
	return;
}

