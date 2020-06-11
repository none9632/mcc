#include "../include/table_names.h"

Name *new_name()
{
	Name *n = malloc(sizeof(Name));
	n->value = 0;
	return n;
}

Name *find(Vector *table_names, char *name)
{
	for (int i = 0; i < table_names->length; ++i)
	{
		Name *n = table_names->data[i];
		if (!strcmp(name, n->name))
			return n;
	}
	return NULL;
}