#ifndef TABLE_H
#define TABLE_H

#include <string.h>

#include "error.h"
#include "vector.h"

typedef struct symbol
{
	char *name;
	int value;
	int type;
}
Symbol;

typedef struct table
{
	Vector *symbols;
	struct table *prev;
}
Table;

Table  *new_table  (Table *prev);
Symbol *new_symbol ();
Symbol *find       (Table *table, char *name);

#endif