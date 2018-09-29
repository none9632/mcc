#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>

#include "error.h"

void error(char* message)
{
	printf("error: %s", message);
	exit(EXIT_FAILURE);
}