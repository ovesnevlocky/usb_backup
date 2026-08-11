#include "path_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>

bool isParentDir(const char *d_name)
{
	if(isNull(d_name))
		return false;

	return strcmp(d_name, "..") == 0;
}	

bool isCurrDir(const char *d_name)
{
	if(isNull(d_name))
		return false;
	return strcmp(d_name, ".") == 0;
}	

bool isHidden(const char *d_name)
{
	if(isNull(d_name))
		return false;

	return d_name[0] == '.';
}

int isPathValid(const char *path)
{
	if(strlen(path) > PATH_MAX)
	{
		fprintf(stderr, "length is over PATH_MAX\n");
		return PATH_TOO_LONG;
	}

	return 0;
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
int cleanDirTo(char *cwd)
{
	int ret = 0;

	ret = isPathValid(cwd);
	if(ret != 0)
		return ret;

	size_t len = strlen(cwd);
	size_t idx = len - 1;

	while(idx > 0 && cwd[idx] !='/')
	{
		cwd[idx] = '\0';
		idx--;
	}

	cwd[idx] = '\0';

	return ret;
}

//inspired by Boyer-Moore algs
bool myStrCmp(const char *path, const char *fname)
{

	if(isNull(path) || isNull(fname))
		return false;

	if(isPathValid(path) != 0 || isPathValid(fname) != 0)
		return false;
	size_t idx = strlen(path);
	size_t idx2 = strlen(fname);

	while(idx > 0  && idx2 > 0)
	{
		if(path[idx - 1] == '/')
			break;
		char p = path[idx-  1];
		char f = fname[idx2 - 1];
		if(p != f)
			return false;
	
		idx--;
		idx2--;
	}


	if(path[idx - 1] == '/' && idx2 == 0)
		return true;

	return false;

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

	if(isPathValid(cwdUsb) != 0 || isPathValid(dir_to) != 0)
		return false;

	//dir_to should be shorter as this is just fname//
	if(len > lenU)
		return false;
	
	//fprintf(stderr, "%s, %s,  %s\n", cwdUsb, cwdUsb + lenU-len, dir_to);
	//return strncmp(cwdUsb + lenU - len, dir_to, len ) == 0;
	return myStrCmp(cwdUsb, dir_to);
	
}


int concat(char *dst, const char *dir_to)
{
	if(isNull(dst) || isNull(dir_to))
	{
		fprintf(stderr, "%s is points to NULL in concat\n", 
				dst == NULL ? "dst" : "dir_to" );
		return 0;
	}
	
	//the first case
	if(strcmp(dir_to, " ") == 0)
	{
		return 0;
	}
		
	size_t len = strlen(dst);
	size_t lenD = strlen(dir_to);
	
	int ret = 0;
	ret = isPathValid(dir_to);
	ret = isPathValid(dst);
	if(ret != 0)
		return ret;

	dst[len] = '/';
	memcpy(dst + len + 1, dir_to, lenD + 1);

	return ret;
}


int  setHome(char *dst, const char *path)
{
	

	int ret = 0;
	ret = isPathValid(path);
	ret = isPathValid(dst);
	if(ret != 0)
		return ret;

	memcpy(dst, path, strlen(path) + 1); 
	return 0;
}

char *cpyPath(const char *path)
{
	if(isNull(path))
		return NULL;

	if(isPathValid(path) != 0)
	{
		return NULL;
	}	

	char *ret = malloc(sizeof(char) * (strlen(path) + 1));
	memcpy(ret, path, strlen(path) + 1);

	return ret;	
}
