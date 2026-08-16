#include <stdio.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>


const char *username()
{
	uid_t uid = getuid();

	struct  passwd *pw = getpwuid(uid);

	return pw == NULL ? NULL : pw->pw_name;


}
int main()
{

	const char *a = username();
	printf("%s\n", a);


	return 0;
}
