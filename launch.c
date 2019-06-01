#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "lib.h"
#include "launch.h"
#include "error.h"

Vector *commands;

// executes commands from the list of commands
static int *CDriver() {
	switch () {
	case CM_NEG:
		break;
	case CM_PLUS:
		break;
	case CM_MINUS:
		break;
	case CM_MULT:
		break;
	case CM_DIV:
		break;
	case CM_MOD:
		break;
	case CM_IF: 
		break;
	case CM_WHILE:
		break;
	case CM_PRINT:
		break;
	case CM_PRINTS:
		break;
	case CM_INPUT:
		break;
}

// runs command execution
void launching_VM(Vector *_commands) {
	commands = _commands;
}
