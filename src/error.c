#include "../include/error.h"

void error(int line, int column, char *format, ...)
{
	va_list arg;
	va_start(arg, format);

	if (line == 0)
	{
		printf("[\033[38;2;255;0;0m");  // for red color
		printf("ERROR");
		printf("\e[0m]: ");
	}
	else
	{
		printf("[\033[38;2;255;0;0m");  // for red color
		printf("ERROR");
		printf("\e[0m]:%i:%i: ", line, column);
	}

	vprintf(format, arg);
	printf("\n");

	exit(EXIT_FAILURE);
}

void func_error()
{
	char *error_buf = strerror(errno);
	error(0, 0, error_buf);
}
