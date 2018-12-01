#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "scan.h"

void error(char* message)
{
	printf("\nerror(%d, %d): %s", posLine, posSym, message);
	exit(EXIT_FAILURE);
}