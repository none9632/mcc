#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#include "vector.h"

// list of token types
enum
{
	TK_IDENT = 128,  // Identifier
	TK_NUM,          // Number literal
	TK_STR,          // String literal
	TK_INC,          // ++
	TK_DEC,          // --
	TK_MOREEQ,       // >=
	TK_LESSEQ,       // <=
	TK_EQUAL,        // == 
	TK_NOTEQ,        // !=
	TK_OR,           // ||
	TK_AND,          // &&
	TK_MULTA,        // *=
	TK_DIVA,         // /=
	TK_PLUSA,        // +=
	TK_MINUSA,       // -=
	TK_MODA,         // %=
	TK_BIT_ANDA,     // &=
	TK_BIT_XORA,     // ^=
	TK_BIT_ORA,      // |=
	TK_IF,           // "if"
	TK_ELSE,         // "else"
	TK_WHILE,        // "while"
	TK_DO,           // "do"
	TK_FOR,          // "for"
	TK_RETURN,       // "return"
	TK_PRINT,        // "print"
	TK_INPUT,        // "input"
	TK_INT,          // "int"
	TK_EOF           // End of file
};

typedef struct token
{
	u_int8_t type;
	int value;

	char *str;             // for storage name variable or string literal
	char *source_line;     // used for displaying an error message

	uint line;
	uint column;
}
Token;

Vector *lexer       (char *file_name);
void    free_tokens (Vector *tokens);

#endif