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

Name *new_name ();
Name *find     (Vector *table_names, char *name);

#endif