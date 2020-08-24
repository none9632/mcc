#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdarg.h>

#include "vector.h"
#include "table.h"
#include "lexer.h"
#include "error.h"
#include "node.h"

Node *parsing (Vector *_tokens);

#endif