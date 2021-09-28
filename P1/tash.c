#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>


void print_error()
{
	char error_message[30] = "An error has occured\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}


void print_2_d_pointers(char **pointer)
{
	printf("Paths are: \n");
	int index = 0;
	while(pointer[index] != NULL)
	{
		printf("%s %zu\n", pointer[index], strlen(pointer[index]));
		index += 1;
	}
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
		arguments[argument_index] = (char *)malloc(1000 * sizeof(char));
	}
	int prev_arg_redir = 0;
	int is_path_op_present = 0;
	char *redir_file = (char *)malloc(1000 * sizeof(char));
	char *dir_path = (char *)malloc(500 * sizeof(char));
	int incorrect_cd_cmd = 0;
	int number_of_cd_arguments = 1;
	int is_more_than_one_redir = 0;

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
			if(command_token != NULL)
			{
				is_more_than_one_redir = 1;
				break;
			}
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
				arguments[resized_arg_index] = (char *)malloc(sizeof(char) * 1000);
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
		if(argument_index <= 0 || is_more_than_one_redir == 1)
		{
			print_error();
			return;
		}
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
	int path_index = 0;
	if(path_set == 1)
	{
		while(paths[path_index] != NULL)
		{
			char *filepath = (char *)malloc(1000 * sizeof(char));
			char *dirpath = (char *)malloc(1000 * sizeof(char));
			strcpy(dirpath, paths[path_index]);
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
			path_index++;
		}
		print_error();
	}
	else
	{
		char *filepath = (char *)malloc(1000 * sizeof(char));
		char* dirpath = (char *)malloc(1000 * sizeof(char));
		strcpy(dirpath, "/bin/");
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
	if(line != NULL)
	{
		char* dup_line = strdup(line);
		dup_line = print_str_prep(dup_line);
		char* delimiter_token = " ";
		char* command_token = strtok(dup_line, delimiter_token);
		if(command_token != NULL)
		{
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
		}
	}
	return 0;
}

char* add_space_on_either_side_of_delim(char *line, char* delim)
{
	char *dupstr = strdup(line);
	char *second_copy = strdup(line);
	int atleast_one_delim_present = 0;
	int atleast_one_char_present = 0;
	char *end = strstr(dupstr, delim);
	char *ret_str = (char *)malloc(25000 * sizeof(char));
	int is_first = 1;
	while(second_copy != NULL && *second_copy != '\0')
	{
		if(*second_copy == delim[0])
		{
			atleast_one_delim_present = 1;
		}
		if(*second_copy != delim[0] && *second_copy != ' ')
		{
			atleast_one_char_present = 1;
		}
		second_copy++;
	}
	if(atleast_one_char_present == 0 && atleast_one_delim_present == 1)
	{
		return dupstr;		
	}
	while(end != NULL)
	{
		size_t len = end - dupstr;
		if(is_first == 1)
		{
			strncpy(ret_str, dupstr, len);
			is_first = 0;
		}
		else
		{
			strncat(ret_str, dupstr, len);
		}
		if(*(end - 1) != ' ')
		{
			strcat(ret_str, " "); 
		}
		strcat(ret_str, delim);
		if(*(end + 1) != '\0' && *(end + 1) != ' ')
		{
			strcat(ret_str, " ");
		}
		if(*(end + 1) == '\0')
		{
			strcat(ret_str, " ");
		}
		dupstr = dupstr + len + strlen(delim);
		end = strstr(dupstr, delim);
	}
	if(is_first == 1)
	{
		return dupstr;
	}
	else
	{
		strcat(ret_str, dupstr);
		return ret_str;
	}
}

int check_num_cd_arguments(char* line)
{
	char* duplicate_str = strdup(line);
	char* local_delimiter = (char*)malloc(1000*sizeof(char));
	strcpy(local_delimiter, " ");
	int arg_count = 0;
	if(duplicate_str != NULL)
	{
		char* command = strtok(line, local_delimiter);
		while(command != NULL)
		{
			arg_count = arg_count + 1;
			command = strtok(NULL, local_delimiter);
		}
	}
	return arg_count;
}

int check_for_cd_func(char* line)
{
	char* dupstr = strdup(line);
	char* duplicate_string = strdup(line);
	if(dupstr != NULL)
	{
		char* delimiter_token = " ";
		char* command_token = strtok(dupstr, delimiter_token);
		if(command_token != NULL)
		{
			if(strcmp(command_token, "cd") == 0)
			{
				command_token = strtok(NULL, delimiter_token);
				if(command_token != NULL)
				{
					int arg_count = check_num_cd_arguments(duplicate_string);
					if(arg_count > 2)
					{
						// int cd_args = arg_count - 1;
						// printf("%d arguments passes to cd.\n", cd_args);
						print_error();
						return 2;
					}
					chdir(command_token);	
					return 1;
				}
			}
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	char** paths;
	int number_of_paths = 1000;
	paths = (char **)malloc(1000 * sizeof(char *));
	int path_index;
	int path_set = 0;
	for(path_index = 0; path_index < 100; path_index++)
	{
		paths[path_index] = (char *)malloc(1000 * sizeof(char));
	}

	if(argc == 1)
	{
		printf("tash> ");
		while((read = getline(&line, &len, stdin)) != -1)
		{
			int check_for_paths = check_for_path(line, paths, number_of_paths);
			if(check_for_paths == 1)
			{
				path_set = 1;
				printf("tash> ");
				continue;
			}
			pid_t wpid;
			if(strcmp(line, "exit\n") == 0 || strcmp(line, "exit") == 0)
			{
				exit(0);
			}
			line = print_str_prep(line);
			line = add_space_on_either_side_of_delim(line, "&");
			char** split_line = split_string_delimiter(line);
			int n_process = get_num_process_to_exec(split_line);
			int string_index = 0;
			int status = 0;
			while(string_index < n_process)
			{
				char *print_str = print_str_prep(split_line[string_index]);
				print_str = add_space_on_either_side_of_delim(print_str, ">");
				int check_for_cd = check_for_cd_func(print_str);
				if(check_for_cd == 0)
				{
					if(strcmp(print_str, "exit") == 0)
					{
						// while((wpid = wait(&status)) > 0);
						exit(0);
					}
					int child_pid = fork();
					if(child_pid == 0)
					{
						process_parallel_executions(print_str, paths, number_of_paths, path_set);
						exit(0);
					}
				}
				string_index += 1;
			}
			while((wpid = wait(&status)) > 0);
			/*
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
			*/
			printf("tash> ");
		}
	}
	else if(argc == 2)
	{
		char* filename = argv[1];
		FILE* file = fopen(filename, "r");
		if(file == NULL)
		{
			print_error();
			return 0;
		}
		while((read = getline(&line, &len, file)) != -1)
		{
			int check_for_paths = check_for_path(line, paths, number_of_paths);
			if(check_for_paths == 1)
			{
				path_set = 1;
				continue;
			}
			pid_t wpid;
			if(strcmp(line, "exit\n") == 0 || strcmp(line, "exit") == 0)
			{
				exit(0);
			}
			line = print_str_prep(line);
			line = add_space_on_either_side_of_delim(line, "&");
			char** split_line = split_string_delimiter(line);
			int n_process = get_num_process_to_exec(split_line);
			int string_index = 0;
			int status = 0;
			while(string_index < n_process)
			{
				char *print_str = print_str_prep(split_line[string_index]);
				print_str = add_space_on_either_side_of_delim(print_str, ">");
				int check_for_cd = check_for_cd_func(print_str);
				if(check_for_cd == 0)
				{
					if(strcmp(print_str, "exit") == 0)
					{
						// while((wpid = wait(&status)) > 0);
						exit(0);
					}
					int child_pid = fork();
					if(child_pid == 0)
					{
						process_parallel_executions(print_str, paths, number_of_paths, path_set);
						exit(0);
					}
				}
				string_index += 1;
			}
			while((wpid = wait(&status)) > 0);
			/*
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
			*/
		}
		
	}
	else
	{
		print_error();
	}
	free(line);
	exit(EXIT_SUCCESS);
}
