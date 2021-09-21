#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

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


		char* delimiter_token = " ";
		char* command_token = strtok(line, delimiter_token);

		int is_redirection_op_present = 0;
		int argument_index = 0;
		int arguments_size = 200;
		char** arguments = (char **)malloc(arguments_size * sizeof(char *));
		for(argument_index = 0; argument_index < arguments_size; argument_index++)
		{
			arguments[argument_index] = (char *)malloc(100 * sizeof(char));
		}
		int prev_arg_redir = 0;
		char *redir_file = (char *)malloc(100 * sizeof(char));
		for(argument_index = 0; command_token != NULL; argument_index++)
		{
			if(strcmp(command_token,">") == 0)
			{
				is_redirection_op_present = 1;
				prev_arg_redir = 1;
				command_token = strtok(NULL, delimiter_token);
				continue;
			}
			if(prev_arg_redir == 1)
			{
				strcpy(redir_file, command_token);
				prev_arg_redir = 0;
				command_token = strtok(NULL, delimiter_token);
				continue;
			}	
			arguments[argument_index] = command_token;
			command_token = strtok(NULL, delimiter_token);
			if(argument_index == arguments_size - 1)
			{
				arguments_size += 100; 
				arguments = (char **)realloc(arguments, arguments_size * sizeof(char *));
				int resized_arg_index;
				for(resized_arg_index = argument_index + 1; resized_arg_index < arguments_size; resized_arg_index++)
				{
					arguments[resized_arg_index] = (char *)malloc(sizeof(char) * 100);
				}
			}
		}
		
		if(is_redirection_op_present == 1)
		{
			close(STDOUT_FILENO);
			open(redir_file, O_CREAT|O_RDWR|O_TRUNC);
			argument_index = argument_index - 2;
		}

		arguments[argument_index] = NULL;

		command_token = strtok(line, delimiter_token);

		if(argument_index == 1)
		{
			command_token[strlen(command_token) - 1] = '\0';
		}
		char *filepath = (char *)malloc(100 * sizeof(char));
		char *dirpath = "/bin/";
		strcpy(filepath, dirpath);
		strcat(filepath, command_token);

		int is_command_executable = access(filepath, X_OK);
		if(is_command_executable == 0)
		{
			int rc = fork();
			if(rc == 0)
			{
				execv(filepath, arguments);
			}
			else
			{
				wait(NULL);
			}
		}
		 
	}
	free(line);
	exit(EXIT_SUCCESS);
}
