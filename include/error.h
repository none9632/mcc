#ifndef ERROR_H
#define ERROR_H

#include "lexer.h"

void error      (Token *token, char *format, ...);
void func_error ();

#endif
