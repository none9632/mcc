#include <stdlib.h>
#include <stdio.h>

void error(char *message, int line, int column)
{
	if (line == 0)
		printf("[ERROR]: %s\n", message);
	else
		printf("[ERROR]:%i:%i: %s\n", line, column, message);
	exit(EXIT_FAILURE);
}
