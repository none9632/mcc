#ifndef LEX_H
#define LEX_H

#include "vector.h"

enum {
	TK_IDENT = 128,  // Identifier
	TK_NUM,          // Number literal
	TK_STR,          // String literal
	TK_MOREEQ,       // >=
	TK_LESSEQ,       // <=
	TK_EQUAL,        // == 
	TK_NOTEQ,        // !=
	TK_MULTA,        // *=
	TK_DIVA,         // /=
	TK_PLUSA,        // +=
	TK_MINUSA,       // -=
	TK_MODA,         // %=
	TK_OR,           // ||
	TK_AND,          // &&
	TK_ELSE,         // "else"
	TK_WHILE,        // "while"
	TK_IF,           // "if"
	TK_PRINT,        // "print"
	TK_FOR,          // "for"
	TK_INPUT,        // "input"
	TK_INT,          // "int"
	TK_CONST,        // "const"
	TK_DO,           // "do"
};

struct token {
	char *name;
	int len;
	int type;
	int value;
	int line;
	int column;
};

typedef struct token Token;

Vector* tokenize (char *fileName);

#endif