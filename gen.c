#include <malloc.h>

#include "gen.h"

Commands *commands;
int cGen = 0; // counter in file gen
int memory = 8192; // amount of memory allocated for the command list

// generates a list of commands
void gen(int command, int *var) {
	if (commands == NULL || cGen >= memory)
		commands = malloc(sizeof(Commands) * memory);
	commands[cGen].command = command;
	commands[cGen++].var = var;
}
