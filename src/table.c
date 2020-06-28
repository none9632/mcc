#include "../include/table.h"

Table *new_table(Table *prev)
{
	Table *table = malloc(sizeof(Table));

	if (table == NULL)
		error(0, 0, "memory allocation error in new_table()");

	table->symbols = new_vector();
	table->prev    = prev;

	return table;
}

Symbol *new_symbol(int type)
{
	Symbol *symbol = malloc(sizeof(Symbol));

	if (symbol == NULL)
		error(0, 0, "memory allocation error in new_symbol()");

	symbol->name  = NULL;
	symbol->value = 0;
	symbol->type  = type;

	return symbol;
}

Symbol *find(Table *table, char *name)
{
	while (table != NULL)
	{
		for (int i = 0; i < table->symbols->length; ++i)
		{
			Symbol *symbol = table->symbols->data[i];
			if (!strcmp(name, symbol->name))
				return symbol;
		}
		table = table->prev;
	}

	return NULL;
}