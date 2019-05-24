#include <stdlib.h>
#include <stdio.h>

#include "lib.h"
#include "lex.h"
#include "pars.h"
#include "launch.h"
#include "error.h"

int main(int argc, char *argv[]) {
	if (argc != 2)
		error("no input files", 0);
	puts("compilation...");
	Vector *tokens = tokenize(argv[1]);
//	parsing(tokens);
//	printCommands();
//	launching();
	exit(EXIT_SUCCESS);
}
