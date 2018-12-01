#define _CRT_SECURE_NO_WARNINGS

#include <malloc.h>

#include "gen.h"

Commands *commands;
int cGen = 0; // counter in file gen
int memory = 8192; 

void gen(int command, int var) {
	if (commands == NULL || cGen >= memory) 
		commands = malloc(sizeof(Commands) * memory);
	commands[cGen].command = command;
	commands[cGen++].var = var;
}