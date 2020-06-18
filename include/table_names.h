#ifndef TABLE_NAMES_H
#define TABLE_NAMES_H

#include <string.h>

#include "vector.h"

typedef struct name
{
	char *name;
	int value;
	int type;
}
Name;

typedef struct table_names
{
	Vector *names;
	struct table_names *prev;
}
Table_names;

Table_names *new_tn   (Table_names *prev);
Name        *new_name ();
Name        *find     (Table_names *tn, char *name);

#endif