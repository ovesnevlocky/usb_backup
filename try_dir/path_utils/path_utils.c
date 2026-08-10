#include "path_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool isParentDir(const char *d_name)
{
	return strcmp(d_name, "..") == 0;
}	

bool isCurrDir(const char *d_name)
{
	return strcmp(d_name, ".") == 0;
}	

bool isHidden(const char *d_name)
{
	return d_name[0] == '.';
}

bool isGit(const char *d_name)
{
	return strcmp(d_name, ".git") == 0;
}	


bool isNull(const void *a)
{
	return a == NULL;
}


	//change back to the string before concating//
void cleanDirTo(char *cwd)
{
	size_t idx = strlen(cwd) - 1 ;

	while(idx > 0 && cwd[idx] !='/')
	{
		cwd[idx] = '\0';
		idx--;
	}
	cwd[idx] = '\0';
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

	//dir_to should be shorter as this is just fname//
	if(len > lenU)
		return false;
	
	//fprintf(stderr, "%s, %s,  %s\n", cwdUsb, cwdUsb + lenU-len, dir_to);
	return strncmp(cwdUsb + lenU - len, dir_to, len ) == 0;
	
}


void concat(char *dst, const char *dir_to)
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
		
	size_t len = strlen(dst);
	dst[len] = '/';
	memcpy(dst + len + 1, dir_to, strlen(dir_to) + 1);

	return;
}


void setHome(char *dst, const char *path)
{
	memcpy(dst, path, strlen(path) + 1); 
}

char *cpyPath(const char *path)
{
	char *ret = malloc(sizeof(char) * (strlen(path) + 1));
	memcpy(ret, path, strlen(path) + 1);

	return ret;	
}
