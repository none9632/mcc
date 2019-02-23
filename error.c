#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "scan.h"

void error(char* message, int typeOutput)
{
	if (typeOutput == 1)
		printf("error(%d, %d): %s\n", posLine, posSym, message);
	else
		printf("error: %s\n", message);
	exit(EXIT_FAILURE);
}
