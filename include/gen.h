#ifndef GEN_H
#define GEN_H

#include <stdio.h>

#include "gen_x86_64.h"
#include "node.h"
#include "error.h"
#include "vector.h"
#include "table.h"

void gen (Node *tree);

#endif
