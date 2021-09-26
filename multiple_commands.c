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
	char* line = NULL;
	char* delimiter = "&";
	char* command = strtok(line, delimiter);
	int num_commands = 200;
	int command_index = 0;
	int induvidual_command_size = 300;
	char** induvidual_commands = (char**)malloc(num_commands * sizeof(char*));
	for(command_index = 0; command_index < num_commands; command_index++)
	{
		induvidual_commands[command_index] = (char*)malloc(induvidual_command_size * sizeof(char));
	}
	command_index = 0;
	while(command != NULL)
	{
		induvidual_commands[command_index] = command;
		command = strtok(NULL, delimiter);
		command_index++;
	}
	return induvidual_commands;
}

int main()
{
	char* input_str;
	size_t len;
	ssize_t read;
	while((read = getline(&input_str, &len, stdin)) != -1)
	{
		char** split_str = split_string_delimiter(input_str);
		printf("%s\n", split_str);
	}
	return 0;
}
