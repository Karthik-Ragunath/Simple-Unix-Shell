#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
	char *dir_changer = "/home/013/k/kx/kxa200005/exercises";
	printf("%d\n", strlen(dir_changer));
	chdir(dir_changer);
	char *myargs[2];
	myargs[0] = "pwd";
	myargs[1] = NULL;
	execv("/bin/pwd", myargs);
	return 0;
}

