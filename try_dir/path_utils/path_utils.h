#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <stdbool.h>
#include <stdlib.h>

enum
{
	PATH_RELATIVE = 10,
	PATH_ABSOLUTE, 
	PATH_NULL = 20,
	BUFF_OVERFLOW,
	BUFF_UNDERFLOW,
	PATH_TOO_LONG,
	PATH_NO_SLASH_AT_START,

};

typedef struct 
{
	char *path;
	//current malloc'd capacity
	//of this array
	size_t size;

}path_t;

bool pathInit(path_t *p);


bool enlargePath(path_t *p, size_t s);

char *getUsrName();


int isPathValid(const char *path, int mode);

bool isNull(const void *a);

int concat(path_t *dst, const char *dir_to);

int cleanDirTo(char *dst);

char *cpyPath(const char *path);

bool isParentDir(const char *d_name);

bool isCurrDir(const char *d_name);

bool myStrCmp(const char *path, const char *fname);

bool isHidden(const char *d_name);

bool isInSameDir(const char *cwdUsb,const char * dir_to);

int setHome(path_t *dst, const char *path);

#endif
