#include "path_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

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

bool enlargePath(path_t *p)
{
	char *tmp = realloc(p->path, p->size * 2);
	if(!tmp)
	{
		perror("realloc");
		return false;
	}

	p->path = tmp;
	p->size *= 2;

	return true;
}

bool pathInit(path_t *p)
{
	p->size = 64;

	p->path = calloc(p->size, 1);
	if(!p->path)
	{
		perror("malloc");
		return false;
	}


	return true;

}

int isPathValid(const char *path, int mode)
{
	if(isNull(path))
		return PATH_NULL;

	if(strlen(path) > PATH_MAX)
	{
		fprintf(stderr, "length is over PATH_MAX\n");
		return PATH_TOO_LONG;
	}

	if(mode == PATH_ABSOLUTE && path[0] != '/')
	{
		fprintf(stderr, "absolute path has to begin with '/'\n");
		return PATH_NO_SLASH_AT_START;
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

	ret = isPathValid(cwd, PATH_ABSOLUTE);
	if(ret != 0)
		return ret;
	
	size_t idx = strlen(cwd);

	while(idx > 0 && cwd[idx - 1] !='/')
	{
		cwd[idx - 1] = '\0';
		idx--;
	}

	cwd[idx - 1] = '\0';

	return ret;
}

//inspired by Boyer-Moore method 
bool myStrCmp(const char *path, const char *fname)
{

	if(isPathValid(path , PATH_ABSOLUTE) != 0 
	|| isPathValid(fname, PATH_RELATIVE) != 0)
		return false;

	size_t idx = strlen(path);
	size_t idx2 = strlen(fname);

	while(idx > 0  && idx2 > 0)
	{
		if(path[idx - 1] == '/')
			break;
		//char p = path[idx-  1];
		//char f = fname[idx2 - 1];
		if(path[idx - 1] != fname[idx2 - 1] )
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

	if(isPathValid(cwdUsb, PATH_ABSOLUTE) != 0 
	|| isPathValid(dir_to, PATH_RELATIVE) != 0)
		return false;

	size_t len = strlen(dir_to);
	size_t lenU = strlen(cwdUsb);


	//dir_to should be shorter as this is just fname//
	if(len > lenU)
		return false;
	
	//return strncmp(cwdUsb + lenU - len, dir_to, len ) == 0;
	return myStrCmp(cwdUsb, dir_to);
	
}


int concat(char *dst, const char *dir_to, size_t size)
{
	//the first case
	//if(strcmp(dir_to, " ") == 0)
	//	return 0;
		
	int ret = 0;

	ret = isPathValid(dir_to, PATH_RELATIVE);
	ret = isPathValid(dst, PATH_ABSOLUTE);

	if(ret != 0)
		return ret;

	if(strcmp(dir_to, " ") == 0)
		return 0;


	//+ 1 for '/', + 1 for null char
	size_t len = strlen(dst);
	size_t lenD = strlen(dir_to);
	if(len + lenD + 2 > PATH_MAX)
		return BUFF_OVERFLOW;

	dst[len] = '/';
	memcpy(dst + len + 1, dir_to, lenD + 1);

	return ret;
}

char * getUsrName()
{
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);

	return pw == NULL ? NULL : pw->pw_name;
}

int  setHome(char *dst, const char *path, size_t size)
{
	int ret = 0;

	ret = isPathValid(path, PATH_ABSOLUTE);
	ret = isPathValid(dst, PATH_RELATIVE);

	if(ret != 0)
		return ret;
	size_t len = strlen(path);
	if(len > size)
		return BUFF_OVERFLOW;

	//if(sizeof(d
	//t) < strlen(path) + 1)
	//	return BUFF_OVERFLOW;
	memcpy(dst, path, len + 1); 
	return ret;
}

//make sure to pass in only absolute path
char *cpyPath(const char *path)
{
	if(isPathValid(path, PATH_RELATIVE) != 0)
		return NULL;

	if(strlen(path) > PATH_MAX)
		return NULL;


	char *ret = malloc(sizeof(char) * (strlen(path) + 1));
	memcpy(ret, path, strlen(path) + 1);

	return ret;	
}
