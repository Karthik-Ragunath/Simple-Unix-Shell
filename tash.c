#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	char** paths;
	int number_of_paths = 100;
	paths = (char **)malloc(100 * sizeof(char *));
	int path_index;
	for(path_index = 0; path_index < 100; path_index++)
	{
		paths[i] = (char *)malloc(100 * sizeof(char));
	}
	printf("tash> ");
	while((read = getline(&line, &len, stdin)) != -1)
	{
		if(strcmp(line, "exit\n") == 0)
		{
			exit(EXIT_SUCCESS);
		}


		char* delimiter_token = " ";
		char* command_token = strtok(line, delimiter_token);

		int is_redirection_op_present = 0;
		int is_change_directory_op_present = 0;
		int argument_index = 0;
		int arguments_size = 200;
		char** arguments = (char **)malloc(arguments_size * sizeof(char *));
		for(argument_index = 0; argument_index < arguments_size; argument_index++)
		{
			arguments[argument_index] = (char *)malloc(100 * sizeof(char));
		}
		int prev_arg_redir = 0;
		int is_path_op_present = 0;
		char *redir_file = (char *)malloc(100 * sizeof(char));
		char *dir_path = (char *)malloc(250 * sizeof(char));
		int incorrect_cd_cmd = 0;
		for(argument_index = 0; command_token != NULL; argument_index++)
		{
			if(argument_index == 0 && strcmp(command_token, "cd") == 0)
			{
				is_change_directory_op_present = 1;
				command_token = strtok(NULL, delimiter_token);
				continue;
			}
			
			if(is_change_directory_op_present == 1)
			{
				strcpy(dir_path, command_token);
				command_token = strtok(NULL, delimiter_token);
				if(command_token != NULL)
				{
					incorrect_cd_cmd = 1;
				}
				break;
			}

			if(strcmp(command_token, "path") == 0 || strcmp(command_token, "path\n") == 0)
			{
				is_path_op_present =  1;
				command_token = strtok(NULL, delimiter_token);
				int path_index = 0;
				for(; command_token != NULL; path_index++)
				{
					paths[path_index] = command_token;
					command_token = strtok(NULL, delimiter_token);
				}
				break;
			}

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

		if(path_op_present == 1)
		{
			printf("Paths set successfully\n");
			printf("tash> ");
			continue;
		}
		
		if(is_change_directory_op_present == 1) 
		{
			if(incorrect_cd_cmd == 1)
			{
				printf("Incorrect cd command\n");
			}
			else
			{
				dir_path[strlen(dir_path) - 1] = '\0';
				printf("dir path len: %d\n", strlen(dir_path));
				printf("The dir path is %s\n", dir_path);
				int dir_change = chdir(dir_path);
				if(dir_change == -1)
				{
					perror("Error: ");
				}
				else if(dir_change == 0)
				{
					printf("dir change is successful. current directory is %s\n", dir_path);
				}
			}

			printf("tash> ");
			continue;
		}

		if(is_redirection_op_present == 1)
		{
			argument_index = argument_index - 2;
		}

		arguments[argument_index] = NULL;

		command_token = strtok(line, delimiter_token);

		if(argument_index == 1 && is_redirection_op_present != 1)
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
				if(is_redirection_op_present == 1)
				{
					redir_file[strlen(redir_file) - 1] = '\0';
					int fd = open(redir_file, O_CREAT | O_WRONLY | O_TRUNC, 00777);
					dup2(fd, STDOUT_FILENO); // make stdout go to file
					dup2(fd, STDERR_FILENO); // make stderr to go to file too
					close(fd);
				}	
				execv(filepath, arguments);
			}
			else
			{
				wait(NULL);
			}
		}
		else
		{
			printf("The given command is not executable\n");
		}
		printf("tash> ");
		 
	}
	free(line);
	exit(EXIT_SUCCESS);
}
