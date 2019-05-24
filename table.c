#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lex.h"
#include "table.h"

StackTok *top;

// will create a new token in the stack
void newToken(char *name, int value, int constType) {
	StackTok *newTok = malloc(sizeof(StackTok));
	newTok->name = malloc(sizeof(char) * 32);
	strcpy(newTok->name, name);
	if (value == 1)
		newTok->value = malloc(sizeof(int));
	else
		newTok->value = NULL;
	newTok->constType = constType;
	newTok->low = top;
	top = newTok;
}

// searches for matches with the name on the stack
StackTok* find(char *name) {
	StackTok *tokObj = top;
	while (tokObj->constType != 2) {
		if (strcmp(name, tokObj->name) == 0)
			break;
		tokObj = tokObj->low;
	}
	if (tokObj->constType == 2)
		error("not identifier defined", 1);
	return tokObj;
}