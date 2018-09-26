#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>

#include "scan.h"
#include "pars.h"
#include "launch.h"

extern FILE* file;

void error(char* message) 
{
	printf("error: %s", message);
	exit(EXIT_FAILURE);
}

void openFile(char* fileName) 
{
	if ((file = fopen(fileName, "r")) == NULL)
		error("file can't open");
}

int main(int argc, char *argv[])
{
	if (argc != 2) 
		error("correct input \n\tmyCompiler.exe <input_file>");
	openFile(argv[1]);
	puts("compilation...");
	scanning();
	parsing();		
	launching();	
	fclose(file);
	exit(EXIT_SUCCESS);
}