#include <stdlib.h>
#include <stdio.h>

#include "../include/vector.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/vm.h"

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