#pragma once 

#include "lib.h"
#include "table.h"

typedef struct token {
	char *name;
	TokenType type;
	int value;
} Token;

// embedded names
typedef struct keyword {
	char *data;
	int type;
} Keyword;

typedef struct symbol {
	char *data;
	int type;
} Symbols;

Vector* tokenize(char *fileName);