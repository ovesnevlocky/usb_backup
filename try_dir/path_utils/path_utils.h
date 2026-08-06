#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <stdbool.h>



bool isNull(const void *a);


void concat(char *dst, const char *dir_to);


void cleanDirTo(char *dst);


char *cpyPath(const char *path);

bool isParentDir(const char *d_name);

bool isCurrDir(const char *d_name);

bool isHidden(const char *d_name);

bool isInSameDir(const char *cwdUsb,const char * dir_to);


bool isInSameDir(const char *cwdUsb,const char * dir_to);


void setHome(char *dst, const char *path);
#endif