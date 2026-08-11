#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <stdbool.h>

enum
{
	PATH_TOO_LONG = 10,


};


int isPathValid(const char *path);

bool isNull(const void *a);


int concat(char *dst, const char *dir_to);


int cleanDirTo(char *dst);


char *cpyPath(const char *path);

bool isParentDir(const char *d_name);

bool isCurrDir(const char *d_name);

bool myStrCmp(const char *path, const char *fname);

bool isHidden(const char *d_name);

bool isInSameDir(const char *cwdUsb,const char * dir_to);


bool isInSameDir(const char *cwdUsb,const char * dir_to);


int setHome(char *dst, const char *path);

#endif
