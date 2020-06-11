#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "vector.h"
#include "table_names.h"
#include "lexer.h"
#include "error.h"
#include "node.h"

Node *parsing(Vector *_tokens);

#endif