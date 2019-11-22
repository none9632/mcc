#ifndef LAUNCH_H
#define LAUNCH_H

#include "table_names.h"

// command list
enum
{
	CM_GET,               // return number value
	CM_GET_TABLE,         // return value from table_names
	CM_NEG,               // negative value
	CM_PLUS,              // +
	CM_MINUS,             // -
	CM_MULT,              // *
	CM_DIV,               // /
	CM_MOD,               // %
	CM_IF,                // "if"
	CM_ELSE,              // "else"
	CM_ELSE_IF,           // "else if"
	CM_DO,                // "do"
	CM_WHILE,             // "while"
	CM_PRINT,             // output expression
	CM_PRINTS,            // output string
	CM_INPUT,             // input
	CM_STORE,             // assigns a value to a variable
	CM_PLUSA,             // +=
	CM_MINUSA,            // -=
	CM_MULTA,             // *=
	CM_DIVA,              // /=
	CM_MODA,              // %=
	CM_ASSIGN,            // =
	CM_EQUAL,             // ==
	CM_NOTEQ,             // !=
	CM_MORE,              // >
	CM_LESS,              // <
	CM_MOREEQ,            // >=
	CM_LESSEQ,            // <=
	CM_AND,               // &&
	CM_OR,                // ||
	CM_STOP,              // point for stop program
	CM_STOP_IF,           // point for stop "if" and "else if"
	CM_STOP_ELSE,         // point for stop "else"
	CM_END_IF,            // point for stop all "if" statement
	CM_STOP_WHILE,        // point for stop "while" and "do while"
	CM_BREAK,             // "break"
	CM_CONTINUE,          // "continue"
};

struct command
{
	char *data;
	Name *table_TN;
	int command;
	int value;
};

typedef struct command Command;

void launching_VM(Vector *_commands);

#endif