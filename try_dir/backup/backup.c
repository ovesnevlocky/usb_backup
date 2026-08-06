#include "backup.h"
#include <stdio.h>
#include <errno.h>
#include "../file_stat/file_stat.h"
#include <time.h>
#include "../usb/usb.h"
#include "../path_utils/path_utils.h"
#include <stdio.h>

#include "../dir_walk/dir_walk.h"
#include <unistd.h>

void startBackUp(usb_t *f, char *cwd, char *usbHome, idxPool_t *p)
{
	uint32_t period = ONEMIN/10;
	errno = 0;
	int count = 0;
		
	while(true)
	{
		checkFiles(f, period, p);
		count++;

		//again check directories//
		if(count > 2)
		{

			fprintf(stderr, "30 sec passed\n");
			setHome(cwd, "/home/kazuy/ws/usb");
			setHome(f->cwdUsb, usbHome);
			openDir(cwd, " ", f, period, p);
			count = 0;
		}

		sleep(period);
	}

}



void checkFiles(usb_t *f, const uint32_t period, idxPool_t *p)
{

	size_t count = f->count;	
	fprintf(stderr, "----------------------------\n");	
	int idx = 0;
	int i = 0;
	while(count > 0)	
	{
		i = p->idxInUse[idx++];

		if(isNull(f->files[i].pathOriginal) && isNull(f->files[i].pathUsb))
		{
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
			//subtract 
			f->byteWritten -= f->files[i].size;

			//push the freed idx;
			stackPush(&p->idxAvailable,  (int)f->files[i].idx);
			errno = 0;
		}
		else if(modified_at)
		{
			//make new copy
			uint64_t bytesWritten = copyFile(f->files[i].pathOriginal, f->files[i].pathUsb, f);
			if(bytesWritten)
			{
				int64_t diff = (int64_t) bytesWritten - (int64_t) f->files[i].size;
				
                int64_t newSize = (int64_t)f->files[i].size + diff;
                int64_t newByteWritten = (int64_t)f->byteWritten + diff;

                f->files[i].size = (uint64_t)newSize;
                f->byteWritten = (uint64_t)newByteWritten;
                
				//(int64_t)f->files[i].size += diff;
				//(int64_t)f->byteWritten += diff;
				f->files[i].modified_at = modified_at;
				//fprintf(stderr, "%s was changed make another copy:%i\n", f->files[i].pathOriginal, diff);
			}
		}
		i++;
		count--;
	}

}


//i dont want to copy if given file is already copied to usb
bool isAlreadyCopied(char *pathUsb)
{
	FILE *fp = fopen(pathUsb, "rb");
	if(fp)
	{
		fprintf(stderr, "given file is already copied to usb, skip\n");
		return true;	
	}
	return false;
}

//copy a file, returns  bytes written  if success, otherwise 0
uint64_t copyFile(const char *cwd, const  char *saveTo, const usb_t * u)
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

		
	FILE *fp_out = fopen(saveTo, "wb");
	if(!fp_out)
	{
		perror(cwd);
		return 0;
	}

	size_t byteWritten, byteRead;
	do
	{
		char buff[4096] = {0};
		byteRead = fread(buff, 1, sizeof(buff), fp);
		if(byteRead <= 0)
			break;
	
		if(isAboveLimit(byteRead, u->limit, u->byteWritten))
		{
			fprintf(stderr, "toomuch data at copyFile\n");
			fclose(fp_out);
			fclose(fp);
			return 0;
		}

		byteWritten = fwrite(buff, 1, byteRead, fp_out);
		if(byteWritten != byteRead)
		{
			printf("written :%lu, read: %lu\n",  byteWritten, byteRead);
			perror("fwrite");
			fclose(fp_out);
			fclose(fp);
			return 0;
		}

	}while(byteRead && fp && fp_out);

	fclose(fp_out);
	fclose(fp);
	return byteWritten;
}




