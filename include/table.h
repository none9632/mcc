#ifndef TABLE_H
#define TABLE_H

#include <string.h>

#include "error.h"
#include "vector.h"

// list of symbols types
enum
{
	S_VAR,
	S_FUNC
};

typedef struct symbol
{
	char *name;
	char *pointer;
	int type;
	int value;
}
Symbol;

typedef struct table
{
	Vector *symbols;
	struct table *prev;
}
Table;

Table  *new_table  (Table *prev);
Symbol *new_symbol (int type);
Symbol *find       (Table *table, char *name);
Symbol *find_all   (Table *table, char *name);

#endif