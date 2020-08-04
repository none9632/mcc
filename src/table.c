#include "../include/table.h"

Table *new_table(Table *prev)
{
	Table *table = malloc(sizeof(Table));

	if (table == NULL)
		func_error();

	table->symbols = new_vector();
	table->prev    = prev;

	return table;
}

Symbol *new_symbol(int type)
{
	static int id_count = 0;

	Symbol *symbol = malloc(sizeof(Symbol));

	if (symbol == NULL)
		func_error();

	symbol->id   = id_count++;
	symbol->name = NULL;
	symbol->type = type;

	return symbol;
}

Symbol *find(Table *table, char *name)
{
	for (int i = 0; i < table->symbols->length; ++i)
	{
		Symbol *symbol = table->symbols->data[i];
		if (!strcmp(name, symbol->name))
			return symbol;
	}

	return NULL;
}

Symbol *find_all(Table *table, char *name)
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