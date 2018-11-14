#define _CRT_SECURE_NO_WARNINGS

#include <malloc.h>

#include "gen.h"

Commands *commands;
int count = 0, memory = 8192;

void gen(int command, int var) {
	if (commands == NULL || count >= memory)
		commands = malloc(sizeof(Commands) * memory);
	commands[count].command = command;
	commands[count++].var = var;
}