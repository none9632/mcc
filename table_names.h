#ifndef TABLE_NAMES_H
#define TABLE_NAMES_H

#include "lib.h"

struct name {
	char   *name;
	int     value;
	int     is_const;
	int     type;
};

typedef struct name Name;

Name *new_name ();
Name *find (Vector *table_names, char *name);

#endif