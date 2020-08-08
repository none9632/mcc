#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

#include "vector.h"
#include "error.h"

// list of token types
enum
{
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
	TK_IF,           // "if"
	TK_ELSE,         // "else"
	TK_WHILE,        // "while"
	TK_DO,           // "do"
	TK_FOR,          // "for"
	TK_RETURN,       // "return"
	TK_PRINT,        // "print"
	TK_INPUT,        // "input"
	TK_INT,          // "int"
	TK_VOID,         // "void"
	TK_EOF           // End of file
};

typedef struct token
{
	int type;
	int value;

	char *str;   // for storage table variable or string literal

	int line;
	int column;
}
Token;

Vector *lexer (char *file_name);

#endif