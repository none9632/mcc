#include <stdlib.h>
#include <stdio.h>

#include "scan.h"
#include "pars.h"
#include "launch.h"
#include "error.h"

FILE* file;

void openFile(char* fileName) {
	if ((file = fopen(fileName, "r")) == NULL)
		error("file can't open", 0);
}

int main(int argc, char *argv[]) {
	if (argc != 2)
		error("no input files", 0);
	openFile(argv[1]);
	puts("compilation...");
	enter();
	CH = fgetc(file);
	bufferCH = fgetc(file);
	nextTok();
	parsing();
	printCommands();
	launching();
	exit(EXIT_SUCCESS);
}
