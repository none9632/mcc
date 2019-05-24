#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "lex.h"

void error(char* message, int typeOutput)
{
	if (typeOutput == 1)
		printf("error(%d, %d): %s\n", 0, 0, message);
	else
		printf("error: %s\n", message);
	exit(EXIT_FAILURE);
}
