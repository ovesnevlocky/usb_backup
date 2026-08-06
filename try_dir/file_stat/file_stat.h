#ifndef FILE_STAT_H
#define FILE_STAT_H
#include <time.h>
#include <stdint.h>


#define ONEMIN 60

#define ONEDAY 24*60*ONEMIN

#define ONEHOUR ONEMIN * 60

#define ONEWEEK 7*ONEDAY


typedef struct
{
	char *pathOriginal;
	char *pathUsb;
	int idx;
	time_t modified_at;
	uint64_t size;
}file_t;



void printStat(const char *path);

time_t getStat(char *path, const uint32_t period);


time_t isModifiedWithinPeriod(time_t lastModified, const uint32_t period);

#endif
