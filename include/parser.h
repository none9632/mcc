#ifndef PARS_H
#define PARS_H

#include <stdio.h>

#include "vector.h"
#include "table_names.h"
#include "lexer.h"
#include "error.h"
#include "vm.h"

Vector *parsing(Vector *_tokens);

#endif