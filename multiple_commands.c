#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>


void process_single_command()
{
	return;
}

char** split_string_delimiter(char* string_input)
{
	char* dup_str = strdup(string_input);
	//char* line = NULL;
	//char* command = strtok(string_input, delimiter);
	char* delimiter = " & ";
	int num_commands = 200;
	int command_index = 0;
	int induvidual_command_size = 300;
	char** induvidual_commands = (char**)malloc(num_commands * sizeof(char*));
	for(command_index = 0; command_index < num_commands; command_index++)
	{
		induvidual_commands[command_index] = (char*)malloc(induvidual_command_size * sizeof(char));
	}
	if(string_input == NULL)
	{
		induvidual_commands[0] = NULL;
		return induvidual_commands;
	}
	char *end = strstr(dup_str, delimiter);
	command_index = 0;
	/*
	printf("Commands are:\n");
	while(command != NULL)
	{
		printf("%s\n", command); 
		induvidual_commands[command_index] = command;
		command = strtok(NULL, delimiter);
		command_index++;
	}
	*/
	
	while(end != NULL)
	{
		size_t len = end - dup_str;
		char* command = (char *)malloc((len + 1) * sizeof(char));
		strncpy(command, dup_str, len);
		strcat(command, "\0");
		induvidual_commands[command_index] = command;
		dup_str = dup_str + len + strlen(delimiter);
		end = strstr(dup_str, delimiter);
		command_index++;
	}
	induvidual_commands[command_index] = dup_str;
	induvidual_commands[command_index + 1] = NULL;
	return induvidual_commands;
}

int main()
{
	char* input_str = NULL;
	size_t len = 0;
	ssize_t read;
	while((read = getline(&input_str, &len, stdin)) != -1)
	{
		char** split_str = split_string_delimiter(input_str);
		int string_index = 0;
		while(split_str[string_index] != NULL)
		{
			printf("%s\n", split_str[string_index]);
			string_index += 1;
		}
	}
	return 0;
}
