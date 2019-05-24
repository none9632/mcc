#pragma once

#define SIZETABLE 9

typedef enum {
	IDENT_TOK = 256,  // Identifier
	NUM_TOK,          // Number literal
	TK_STR,           // String literal
	MOREEQ_TOK,       // >=
	LESSEQ_TOK,       // <=
	EQUAL_TOK,        // == 
	NOTEQ_TOK,        // !=
	MULTA_TOK,        // *=
	DIVA_TOK,         // /=
	PLUSA_TOK,        // +=
	MINUSA_TOK,       // -=
	MODA_TOK,         // %=
	ELSE_TOK,         //
	WHILE_TOK,        //
	IF_TOK,           //
	PRINT_TOK,        //
	TK_FOR,           //
	INPUT_TOK,        //
	TK_INT,           //
	TK_DOUBLE,        //
	TK_CONST,         //
	TK_BOOL,          //
	TK_CHAR,          //
	TK_DO,            //
} TokenType;

typedef enum {
	intType, 
	noneType, 
	constType, 
	boolType, 
	doubleType
} TypeVar;

struct embeddedNames {
	char name[7];
	TokenType type;
} TableNames[SIZETABLE];

typedef struct Token {
	char *name;
	int *value;
	int constType;
	struct Token* low;
} StackTok;

void newToken(char *name, int value, int constType);
int searchTN(char *name);
StackTok* find(char *name);
TypeVar findType(char *name);
void enter();