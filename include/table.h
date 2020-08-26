#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>

#include "vector.h"

// list of symbols types
enum
{
	S_VAR,
	S_FUNC,
};

typedef struct symbol
{
	char *name;
	char *pointer;
	int8_t type;
	int value;          // used for counting the number of parameters in the function
}
Symbol;

typedef struct table
{
	Vector *symbols;
	struct table *prev;
}
Table;

Table  *new_table  (Table *prev);
Symbol *new_symbol (int8_t type, char *name, Table *sym_tab);
Symbol *find       (Table *table, char *name);
Symbol *find_all   (Table *table, char *name);

#endif