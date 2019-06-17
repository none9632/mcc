#ifndef LAUNCH_H
#define LAUNCH_H

// command list
enum
{
	CM_GET,               // 
	CM_GET_TABLE,         // 
	CM_NEG,               // 
	CM_PLUS,              // 
	CM_MINUS,             // 
	CM_MULT,              // 
	CM_DIV,               // 
	CM_MOD,               // 
	CM_IF,                // 
	CM_ELSE,              // 
	CM_ELSE_IF,           // 
	CM_DO,                // 
	CM_WHILE,             // 
	CM_PRINT,             // 
	CM_PRINTS,            // 
	CM_INPUT,             // 
	CM_STORE,             // 
	CM_PLUSA,             // 
	CM_MINUSA,            // 
	CM_MULTA,             // 
	CM_DIVA,              // 
	CM_MODA,              // 
	CM_ASSIGN,            // 
	CM_EQUAL,             // 
	CM_NOTEQ,             // 
	CM_MORE,              // 
	CM_LESS,              // 
	CM_MOREEQ,            // 
	CM_LESSEQ,            // 
	CM_AND,               // 
	CM_OR,                // 
	CM_STOP,              // 
	CM_STOP_IF,           // 
	CM_STOP_ELSE,         // 
	CM_END_IF,            // 
	CM_STOP_WHILE,        // 
	CM_STOP_FOR,          // 
	CM_BREAK,             // 
	CM_CONTINUE           // 
};

void launching_VM(Vector *_commands);

#endif