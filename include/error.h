#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void error (int line, int column, char *format, ...);

#endif
