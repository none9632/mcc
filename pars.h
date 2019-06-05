#ifndef PARS_H
#define PARS_H

#include "lib.h"
#include "table_names.h"

struct command {
	char   *data;
	Name   *table_TN;
	int     command;
	int     value;
};

typedef struct command Command;

Vector* parsing(Vector *_table_names, Vector *_tokens);

#endif