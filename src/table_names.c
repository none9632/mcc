#include <stdlib.h>
#include <string.h>

#include "table_names.h"

Table_N *new_table_n(Table_N *prev)
{
	Table_N *tn = malloc(sizeof(Table_N));
	tn->names = new_vec();
	tn->prev = prev;
	return tn;
}

Name *new_name()
{
	Name *n = malloc(sizeof(Name));
	n->is_const = 0;
	n->value = 0;
	return n;
}

Name *find(Table_N *table_names, char *name)
{
	while (table_names != NULL)
	{
		for (int i = 0; i < table_names->names->length; ++i)
		{
			Name *n = table_names->names->data[i];
			if (!strcmp(name, n->name))
				return n;
		}
		table_names = table_names->prev;
	}
	return NULL;
}