#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "node.h"
#include "gen.h"

static int8_t help = 0;
static int8_t print_ast = 0;
static char *file_name = NULL;

static void usage()
{
	printf("Usage: mcc [options] <file>\n");
	printf("Options:\n");
	printf("    --help          display this information\n");
	printf("    --print-ast     display AST\n");
	printf("\n");
	exit(EXIT_SUCCESS);
}

static void parse_options(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		char *opt = argv[i];

		if (*opt == '-')
		{
			if (!strcmp(opt, "--help"))
				help = 1;
			else if (!strcmp(opt, "--print-ast"))
				print_ast = 1;
			else
				error(NULL, "invalid option");
		}
		else
		{
			if (file_name == NULL)
				file_name = opt;
			else
				error(NULL, "invalid input");
		}
	}
}

int main(int argc, char **argv)
{
	parse_options(argc, argv);

	if (help == 1)
		usage();

	if (file_name == NULL)
		error(NULL, "no input file");

	Vector *tokens = lexer(file_name);
	Node *tree = parsing(tokens);

	if (print_ast == 1)
		start_print_node(tree);

	gen(tree);

	exit(EXIT_SUCCESS);
}
