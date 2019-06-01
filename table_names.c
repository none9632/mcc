#include <stdlib.h>
#include <string.h>

#include "table_names.h"
#include "error.h"

Name *new_name()
{
	Name *n = malloc(sizeof(Name));
	n->is_const = 0;
	n->value = 0;
	return n;
}

Name *find(Vector *table_names, char *name)
{
	for (int i = 0; i < table_names->len; i++) {
		Name *v = table_names->data[i];
		if (!strcmp(name, v->name))
			return v;
	}
	return NULL;
}