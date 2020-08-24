#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "lexer.h"

void error      (Token *token, char *format, ...);
void func_error ();

#endif
