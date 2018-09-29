#ifndef TABLE_H
#define TABLE_H

#include "scan.h"

struct Token 
{
	char name[NAMESIZE];
	int value;
	TokenType tokenType;
	struct Token* low;
};

void newToken(void);

#endif