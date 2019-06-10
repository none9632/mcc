#ifndef TABLE_NAMES_H
#define TABLE_NAMES_H

#include "vector.h"

struct name {
	char *name;
	int value;
	int is_const;
	int type;
};

struct table_names {
	Vector *names;
	struct table_names *prev;
};

typedef struct name Name;
typedef struct table_names Table_N;

Table_N* new_table_n(Table_N *prev);
Name* new_name();
Name* find(Table_N *table_names, char *name);

#endif