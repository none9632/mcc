#include <stdlib.h>
#include <stdio.h>

#include "../include/vector.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/node.h"

static int   help      = 0;
static int   print_ast = 0;
static char *file_name = NULL;

static void usage()
{
	printf("Usage: myCompiler [options] <file>");
	printf("\nOptions:\n");
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
			{
				printf("error: invalid option\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if (file_name == NULL)
				file_name = opt;
			else
			{
				printf("error: invalid input\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

int main(int argc, char **argv)
{
	parse_options(argc, argv);

	if (help == 1)
		usage();

	if (file_name == NULL)
	{
		printf("error: No input file\n");
		exit(EXIT_FAILURE);
	}

	Vector *tokens = lexer(file_name);
	Node   *tree   = parsing(tokens);

	if (print_ast == 1)
		start_print_node(tree);

	exit(EXIT_SUCCESS);
}