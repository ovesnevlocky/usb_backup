#include "backup.h"
#include <stdio.h>
#include <errno.h>
#include "../file_stat/file_stat.h"
#include <time.h>
#include "../usb/usb.h"
#include "../path_utils/path_utils.h"
#include <stdio.h>
#include "../mem_pool/mem_pool.h"
#include "../dir_walk/dir_walk.h"
#include <unistd.h>
#include <assert.h>
#include <limits.h>


bool removeFile(const char *path)
{
	fprintf(stderr, "deleting %s\n", path);
    	return remove(path) == 0;
}


void startBackUp(usb_t *f, const char *cwdHome,path_t *cwd, char *usbHome, idxPool_t *p)
{
	uint32_t period = ONEMIN/10;

	errno = 0;
	int count = 0;
	int count_ = 0;	

	while(true && isUsbMounted())
	{
		checkFiles(f, period, p);
		count++;
		//again check directories//
		if(count > 2)
		{

			fprintf(stderr, "30 sec passed\n");
			setHome(cwd, cwdHome);
			setHome(&f->cwd, usbHome);
			openDir(cwd, " ", f, ONEMIN/2, p);
			count = 0;
			count_++;
		}

		sleep(period);
		if(count_ > 7)
			return;
	}

}

void printCheck(const char *path1, const char *path2)
{
	if(!isNull(path1))
		fputs(path1, stderr);
	if(!isNull(path2))
		fputs(path2, stderr);

}

void checkFiles(usb_t *f, const uint32_t period, idxPool_t *p)
{
	
	//as we want to walk through all the files we have at this point
	size_t count = f->count;	
	fprintf(stderr, "----------------------------\n");	


	if(f->count != p->count)
		fprintf(stderr, "counts dont agree . ... p: %lu, u: %lu\n", p->count, f->count);

	for(size_t i = 0;  count > 0 ; i++)
	{
	
		if(p->idxInUse[i] == false)
		{
			
			if(isNull(f->files[i].pathOriginal) && isNull(f->files[i].pathUsb))
			{
				continue;
			}
			else
			{
				fprintf(stderr, "not in use but points to some??");
				printCheck(f->files[i].pathOriginal, f->files[i].pathUsb);
				continue;
			}
		}

		errno = 0;
		time_t modified_at = getStat(f->files[i].pathOriginal, period);
		fprintf(stderr, "checking %s\n", f->files[i].pathOriginal);
		//removed, moved, .....
		if(errno == ENOENT)
		{

			fprintf(stderr, "%i: %s points to NULL\n"
					,f->files[i].idx,  f->files[i].pathOriginal);

            		if(!removeFile(f->files[i].pathUsb))
                		perror("remove");
                
			free(f->files[i].pathOriginal);
			free(f->files[i].pathUsb);
			f->files[i].pathOriginal = NULL;
			f->files[i].pathUsb = NULL;
			p->count -= 1;
			f->count -= 1;

			//subtract 
			int64_t newSize  = (int64_t) f->byteWritten - (int64_t) f->files[i].size;
			if(newSize < 0)
				newSize = 0;

			f->byteWritten = (uint64_t) newSize;
			
			//push the freed idx;
			if(isStackFull(&p->idxAvailable))
			{
				int ret = enlargePool(p);
				if(ret != 0)
					fprintf(stderr, "error at engargepool at checkFiles\n");
			}

			stackPush(&p->idxAvailable,  (int)f->files[i].idx);
			//setfalse
			p->idxInUse[f->files[i].idx] = false;
		}
		else if(modified_at)
		{
			//make new copy
			uint64_t bytesWritten = copyFile(f->files[i].pathOriginal, f->files[i].pathUsb, f);
			if(bytesWritten)
			{
				//diff of the 2 same files
				int64_t diff = (int64_t) bytesWritten - (int64_t) f->files[i].size;

				//compute new byteWritten of entire files//
                		int64_t newByteWritten = (int64_t)f->byteWritten + diff;
			
				if(newByteWritten < 0)
					newByteWritten = 0;

                		f->files[i].size = bytesWritten;
					
                		f->byteWritten = newByteWritten;
				
				//renew the modified time
				f->files[i].modified_at = modified_at;
			}
		}
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
		return fclose(fp) == 0;	
	}
	return false;
}

//copy a file, returns  bytes written  if success, otherwise 0
int64_t copyFile(const char *cwd, const  char *saveTo, const usb_t * u)
{
	if(isNull(cwd) || isNull(saveTo))
	{
		fprintf(stderr, "%s is points to NULL at copyFile\n", 
				cwd == NULL ? "cwd" : "saveTo" );
		return INVALID_PATH;
	}

	FILE *fp = fopen(cwd, "rb");
	if(!fp)
	{
		perror(cwd);
		return ERROR_OPEN_F;
	}

	FILE *fp_out = fopen(saveTo, "wb");
	if(!fp_out)
	{
		perror(cwd);
		return ERROR_OPEN_F;
	}
	
	size_t bytesWritten = 0;
	size_t byteWritten = 0, byteRead = 0;
	do
	{
		char buff[4096] = {0};
		byteRead = fread(buff, 1, sizeof(buff), fp);
		if(byteRead <= 0)
			break;
	
		if(isAboveLimit(byteRead + bytesWritten , u->limit, u->byteWritten))
		{
			fprintf(stderr, "toomuch data at copyFile\n");
			fclose(fp_out);
			fclose(fp);
			return ABOVE_LIMIT_USB;
		}

		byteWritten = fwrite(buff, 1, byteRead, fp_out);
		if(byteWritten != byteRead)
		{
			printf("written :%lu, read: %lu\n",  byteWritten, byteRead);
			perror("fwrite");
			fclose(fp_out);
			fclose(fp);
			return FAILURE_IN_WRITING;
		}

		bytesWritten += byteWritten;

	}while(byteRead && fp && fp_out);

	if(fclose(fp_out) != 0 || fclose(fp) != 0)
			return ERROR_CLOSE_F;

	return bytesWritten;
}


