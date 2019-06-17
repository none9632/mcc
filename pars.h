#ifndef PARS_H
#define PARS_H

#include "vector.h"
#include "table_names.h"

struct command
{
	char *data;
	Name *table_TN;
	int command;
	int value;
};

typedef struct command Command;

Vector *parsing(Vector *_tokens);

#endif