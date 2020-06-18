#include "../include/table_names.h"

Table_names *new_tn(Table_names *prev)
{
	Table_names *tn = malloc(sizeof(Table_names));

	tn->names = new_vec();
	tn->prev  = prev;

	return tn;
}

Name *new_name(int type)
{
	Name *n = malloc(sizeof(Name));

	n->name  = NULL;
	n->value = 0;
	n->type  = type;

	return n;
}

Name *find(Table_names *tn, char *name)
{
	while (tn != NULL)
	{
		for (int i = 0; i < tn->names->length; ++i)
		{
			Name *n = tn->names->data[i];
			if (!strcmp(name, n->name))
				return n;
		}

		tn = tn->prev;
	}

	return NULL;
}