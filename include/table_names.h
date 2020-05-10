#ifndef TABLE_NAMES_H
#define TABLE_NAMES_H

#include <string.h>

#include "vector.h"

typedef struct name
{
	char *name;
	int value;
	int is_const;
	int type;
}
Name;

typedef struct table_names
{
	Vector *names;
	struct table_names *prev;
}
Table_N;

Table_N *new_table_n(Table_N *prev);
Name *new_name();
Name *find(Table_N *table_names, char *name);

#endif