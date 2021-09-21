#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main()
{
	printf("This is a sample execution\n");
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	while((read = getline(&line, &len, stdin)) != -1)
	{
		if(strcmp(line, "exit\n") == 0)
		{
			printf("breaking out");
			exit(EXIT_SUCCESS);
		}
		printf("The input is %s\n", line);
		printf("The retrieved length is %d\n", read);
		char* delimiter_token = " ";
		char* token = strtok(line, delimiter_token);
		int i;
		for(i = 0; token != NULL; i++)
		{
			if(i == 0)
			{
				printf("The command given is %s\n", token);
			}
			else
			{
				printf("The argument %d is %s\n", i, token); 
			}
			token = strtok(NULL, delimiter_token);
		}

		int is_executable = access("/bin/ls", X_OK);
		if(is_executable == 0)
		{
			printf("Yes, it is executable\n");
		}
		else if(is_executable == -1)
		{
			printf("No, the specified file path is not a executable\n");
		}

		char* command_token = strtok(line, delimiter_token);
		int is_command_executable = access(command_token, X_OK);
		if(is_command_executable == 0)
		{
			char* arguments;
			int argument_size = 100;
			arguments = (char *)malloc(argument_size);
			int argument_index;
			for(argument_index = 0; command_token != NULL; argument_index++)
			{
				command_token = strtok(NULL, delimiter_token);
				arguments[i] = command_token;
				if(argument_index == argument_size - 2)
				{
					argument_size += 100;
					arguments = (char*)realloc(argument_size);
				}					
			}
			arguments[argument_index + 1] = NULL;
			execv(
		}
	}
	free(line);
	exit(EXIT_SUCCESS);
}
