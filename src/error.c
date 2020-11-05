#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "../include/lexer.h"

void error(Token *token, char *format, ...)
{
	const char *red = "\033[38;2;255;0;0m";
	const char *end = "\e[0m";

	va_list arg;
	va_start(arg, format);

	if (token == NULL)
	{
		printf("[%sERROR%s] ", red, end);
		vprintf(format, arg);
		printf("\n");
	}
	else
	{
		printf("[%sERROR%s]", red, end);
		printf(":%u:%u: ", token->line, token->column);
		vprintf(format, arg);
		printf("\n");

		printf("%s\n", token->source_line);

		for (int i = 0; token->source_line[i] != '\0'; ++i)
		{
			if (i + 1 == token->column)
				printf("^");
			else if (token->source_line[i] == '\t')
				printf("\t");
			else
				printf(" ");
		}

		printf("\n");
	}

	exit(EXIT_FAILURE);
}

void func_error()
{
	char *error_buf = strerror(errno);
	error(NULL, error_buf);
}
