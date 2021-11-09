#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

void print_error()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void process_parallel_executions(char* line, char** paths, int number_of_paths, int path_set)
{
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
	int number_of_cd_arguments = 1;
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
				number_of_cd_arguments += 1;
				continue;
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
			paths[path_index] = NULL;
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

	if(is_path_op_present == 1)
	{
		printf("Paths set successfully:\n");
		int path_index = 0;
		while(paths[path_index] != NULL)
		{
			printf("%s\n", paths[path_index]);
			path_index++;
		}
		return;
	}
	
	if(is_change_directory_op_present == 1) 
	{
		if(incorrect_cd_cmd == 1)
		{
			// printf("%d arguments are passed to cd.\n", number_of_cd_arguments);
			print_error();
		}
		else
		{
			if(dir_path[strlen(dir_path) - 1] == '\n')
			{
				dir_path[strlen(dir_path) - 1] = '\0';
			}
			int dir_change = chdir(dir_path);
			if(dir_change == -1)
			{
				// perror("Error: ");
				print_error();
			}
			if(dir_change == 0)
			{
				// printf("%s\n", dir_path);
				exit(10);
			}
		}

		return;
	}

	if(is_redirection_op_present == 1)
	{
		argument_index = argument_index - 2;
	}

	arguments[argument_index] = NULL;
	command_token = strtok(line, delimiter_token);

	if(argument_index == 1 && is_redirection_op_present != 1)
	{
		if(command_token[strlen(command_token) - 1] == '\n')
		{
			command_token[strlen(command_token) - 1] = '\0';
		}
	}
	char *filepath = (char *)malloc(100 * sizeof(char));
	int path_index = 0;
	if(path_set == 1)
	{
		while(paths[path_index] != NULL)
		{
			char *dirpath = paths[path_index];
			strcat(dirpath, "/");
			strcpy(filepath, dirpath);
			strcat(filepath, command_token);

			int is_command_executable = access(filepath, X_OK);
			if(is_command_executable == 0)
			{
				if(is_redirection_op_present == 1)
				{
					if(redir_file[strlen(redir_file) - 1] == '\n')
					{
						redir_file[strlen(redir_file) - 1] = '\0';
					}
					int fd = open(redir_file, O_CREAT | O_WRONLY | O_TRUNC, 00777);
					dup2(fd, STDOUT_FILENO); // make stdout go to file
					dup2(fd, STDERR_FILENO); // make stderr to go to file too
					close(fd);
				}	
				execv(filepath, arguments);
			}
			path_index += 1;
		}
		print_error();
	}
	else
	{
		char *dirpath = "/bin/";
		strcpy(filepath, dirpath);
		strcat(filepath, command_token);

		int is_command_executable = access(filepath, X_OK);
		if(is_command_executable == 0)
		{
			if(is_redirection_op_present == 1)
			{
				if(redir_file[strlen(redir_file) - 1] == '\n')
				{
					redir_file[strlen(redir_file) - 1] = '\0';
				}
				int fd = open(redir_file, O_CREAT | O_WRONLY | O_TRUNC, 00777);
				dup2(fd, STDOUT_FILENO); // make stdout go to file
				dup2(fd, STDERR_FILENO); // make stderr to go to file too
				close(fd);
			}	
			execv(filepath, arguments);
		}
		else
		{
			print_error();
		}
	}
	return;
}

char** split_string_delimiter(char* string_input)
{
	char* dup_str = strdup(string_input);
	char* delimiter = " & ";
	int num_commands = 200;
	int command_index = 0;
	int induvidual_command_size = 300;
	char** induvidual_commands = (char **)malloc(num_commands * sizeof(char *));
	for(command_index = 0; command_index < num_commands; command_index++)
	{
		induvidual_commands[command_index] = (char *)malloc(induvidual_command_size*sizeof(char));
	}
	if(string_input == NULL)
	{
		induvidual_commands[0] = NULL;
		return induvidual_commands;
	}
	char *end = strstr(dup_str, delimiter);
	command_index = 0;
	while(end != NULL)
	{
		size_t len = end - dup_str;
		char* command = (char *)malloc((len + 1) * sizeof(char));
		strncpy(command, dup_str, len);
		strcat(command, "\0");
		induvidual_commands[command_index] = command;
		dup_str = dup_str + len + strlen(delimiter);
		end = strstr(dup_str, delimiter);
		command_index += 1;
	}
	induvidual_commands[command_index] = dup_str;
	induvidual_commands[command_index + 1] = NULL;
	return induvidual_commands;
}

char* print_str_prep(char *input)
{
	if(input != NULL)
	{
		if(input[strlen(input) - 1] == '\n')
		{
			char *dupstr = strdup(input);
			dupstr[strlen(dupstr) - 1] = '\0';
			return dupstr;
		}
	}
	return input;

}

int get_num_process_to_exec(char** two_d_pointer_array)
{
	int n_process = 0;
	while(two_d_pointer_array[n_process] != NULL)
	{
		n_process += 1;
	}
	return n_process;
}

int check_for_path(char* line, char** paths, int number_of_paths)
{
	char* dup_line = strdup(line);
	dup_line = print_str_prep(dup_line);
	char* delimiter_token = " ";
	char* command_token = strtok(dup_line, delimiter_token);
	if(strcmp(command_token, "path") == 0)
	{
		command_token = strtok(NULL, delimiter_token);
		int path_index = 0;
		for(; command_token != NULL; path_index++)
		{
			paths[path_index] = command_token;
			command_token = strtok(NULL, delimiter_token);
		}
		paths[path_index] = NULL;
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	char** paths;
	int number_of_paths = 100;
	paths = (char **)malloc(100 * sizeof(char *));
	int path_index;
	int path_set = 0;
	for(path_index = 0; path_index < 100; path_index++)
	{
		paths[path_index] = (char *)malloc(100 * sizeof(char));
	}

	if(argc == 1)
	{
		printf("tash> ");
		while((read = getline(&line, &len, stdin)) != -1)
		{
			pid_t child_pid, wpid;

			if(strcmp(line, "exit\n") == 0 || strcmp(line, "exit") == 0)
			{
				exit(EXIT_SUCCESS);
			}
			char** split_line = split_string_delimiter(line);
			int n_process = get_num_process_to_exec(split_line);
			int string_index = 0;
			/*
			while(split_line[string_index] != NULL)
			{
				char *print_str = print_str_prep(split_line[string_index]);
				process_single_command(print_str, paths, number_of_paths);
				string_index += 1;
			}
			*/
			int status = 0;
			while(string_index < n_process)
			{
				int child_pid = fork();
				if(child_pid == 0)
				{
					char *print_str = print_str_prep(split_line[string_index]);
					process_parallel_executions(print_str, paths, number_of_paths, path_set);
					exit(0);
				}
				string_index += 1;
			}
			while((wpid = wait(&status)) > 0);
			printf("tash> ");
		}
	}
	else if(argc == 2)
	{
		char* filename = argv[1];
		FILE* file = fopen(filename, "r");
		while((read = getline(&line, &len, file)) != -1)
		{
			int check_for_paths = check_for_path(line, paths, number_of_paths);
			if(check_for_paths == 1)
			{
				path_set = 1;
				continue;
			}
			pid_t child_pid, wpid;
			if(strcmp(line, "exit\n") == 0 || strcmp(line, "exit") == 0)
			{
				exit(EXIT_SUCCESS);
			}

			char** split_line = split_string_delimiter(line);
			int n_process = get_num_process_to_exec(split_line);
			int string_index = 0;
			int status = 0;
			while(string_index < n_process)
			{
				int child_pid = fork();
				if(child_pid == 0)
				{
					char *print_str = print_str_prep(split_line[string_index]);
					process_parallel_executions(print_str, paths, number_of_paths, path_set);
					exit(0);
				}
				string_index += 1;
			}
			// while((wpid = wait(&status)) > 0);

			while((wpid = wait(&status)) > 0)
			{
				int exact_value_sent = (int)(status / 255);
				if(exact_value_sent == 10)
				{
					if(n_process == 1)
					{
						char* print_str = print_str_prep(split_line[0]);
						char* delimiter = " ";
						char* command = strtok(print_str,delimiter);
						command = strtok(NULL, delimiter);
						chdir(command);
					}
				}
			}
		}
		
	}
	else
	{
		printf("More than 2 input batch files. Not valid\n");
	}
	free(line);
	exit(EXIT_SUCCESS);
}
