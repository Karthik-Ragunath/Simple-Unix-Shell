#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<unistd.h>
#include<fcntl.h>
int main()
{
	int rc = fork();
	if(rc == 0)
	{
		close(STDOUT_FILENO);
		open("output.txt", O_CREAT|O_WRONLY|O_TRUNC);
		char *myarguments[2];
		myarguments[0] = "ls";
		myarguments[1] = NULL;
		execv("/bin/ls", myarguments);
	}
	else
	{
		wait(NULL);
	}
	return 0;
}
