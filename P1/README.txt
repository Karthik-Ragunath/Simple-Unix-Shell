High Level Code WalkThrough

Different Methods and their purposes:

1. check_for_path() - To check if path command is specified
2. add_space_on_either_side_of_delim(), split_string_delimiter(), get_num_process_to_exec() - Preprocessing the given bash command before execution
3. check_for_cd_func(), get_num_cd_arguments() - To perform cd built-in function and to identify possible errors
4. process_parallel_executions() - Function called to perform forking and parallel execution operations
5. print_2_d_pointers() - For internal debugging purpose
6. print_error() - Function called for printing to STDERR whenever we catch errors and exceptions

