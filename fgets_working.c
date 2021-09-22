#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
	char* line = (char *)malloc(50 * sizeof(char));
	int len = 50;
	char* read;
	ssize_t read_getline;
	
	while(fgets(line, len, stdin) != NULL)
	{
		printf("The string len via fgets is %d\n", strlen(line));
		break;
	}
	
	line = NULL;
	size_t len_getline = 0;
	
	while((read_getline = getline(&line, &len_getline, stdin)) != -1)
	{
		printf("The string len is %d\n", strlen(line));
		char* delimiter_token = " ";
		char* token = strtok(line, delimiter_token);
		if(strcmp(token, "path\n") == 0)
		{
			printf("just checking the working of strtok, works as expected\n");
		}
		else
		{
			printf("lol, its not as expected\n");
		}
		break;
	}
	return 0;
}
