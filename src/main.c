#include <stdlib.h>
#include <stdio.h>

#include "vector.h"
#include "lexer.h"
#include "parser.h"
#include "launch.h"

void usage()
{
	printf("Usage: myCompiler <file>\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		usage();

	Vector *tokens = lexer(argv[1]);
	Vector *commands = parsing(tokens);
	launching_VM(commands);

	exit(EXIT_SUCCESS);
}