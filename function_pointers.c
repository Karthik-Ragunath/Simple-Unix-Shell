#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

void modify_double_pointers(char** double_p)
{
	double_p[0] = "Karthik Ragunath";			
	return;
}


int main()
{
	char** double_p = (char **)malloc(50 * sizeof(char*));
	int i, j;
	for(i = 0; i < 50; i++)
	{
		double_p[i] = (char *)malloc(50 * sizeof(char));
	}
	modify_double_pointers(double_p);
	printf("%s", double_p[0]);
	return 0;
}
