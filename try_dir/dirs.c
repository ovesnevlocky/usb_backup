#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>



bool isParentDir(char *d_name)
{
	return strcmp(d_name, "..") == 0;
}	

bool isCurrDir(char *d_name)
{
	return strcmp(d_name, ".") == 0;
}	


void openDir(DIR *dirp, struct dirent *dp)
{
	bool isParentDirSeen = false;
	DIR *dir; 
	while(dirp)
	{
		errno = 0;
		dp = readdir(dirp);
		if(dp == NULL)
		{
			if(errno !=  0)
				perror("readdir");
			break;	
		}			
	
		if(dp ->d_type == DT_DIR)
		{	
			if(isParentDir(dp->d_name))
			{
				if(!isParentDirSeen)
				{
					isParentDirSeen = true;
					continue;
				}

			}
			if(isCurrDir(dp->d_name))
				continue;	
			printf("open dir: %s\n", dp->d_name);
			dir = opendir(dp->d_name);
			openDir (dir, dp);
		}
		else if(dp ->d_type == DT_REG)
		{
			printf("file: %s\n", dp->d_name);
			
		}
	}	
	return;
}



int main()
{
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	
	DIR *dirp = opendir(cwd);
	struct dirent *dp;
	openDir(dirp, dp);
	
	closedir(dirp);
	

}
