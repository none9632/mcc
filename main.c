#include <stdlib.h>
#include <stdio.h>

#include "lib.h"
#include "lex.h"
#include "pars.h"
#include "launch.h"

void usage()
{
	printf("Usage: myCompiler <file>\n");
	exit(EXIT_FAILURE);
}

void main(int argc, char *argv[])
{
	if (argc != 2)
		usage();

	Vector *table_names = new_vec();
	Vector *tokens = tokenize(argv[1]);
	Vector *commands = parsing(table_names, tokens);
	launching_VM(commands);

	exit(EXIT_SUCCESS);
}