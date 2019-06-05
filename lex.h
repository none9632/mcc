#ifndef LEX_H
#define LEX_H

#include "lib.h"

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
	TK_BOOL,         // "bool"
	TK_CHAR,         // "char"
	TK_DO,           // "do"
};

struct token {
	char   *name;
	int     type;
	int     value;
};
struct keyword {
	char   *data;
	int     type;
};

typedef struct token   Token;
typedef struct keyword Keyword;


Vector* tokenize (char *fileName);

#endif