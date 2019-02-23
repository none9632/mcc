#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "scan.h"
#include "table.h"

StackTok *top;

void enterTableNames(char *name, TokenType type) {
	static int count = 0;
	strcpy(TableNames[count].name, name);
	TableNames[count++].type = type;
}

// will create a new token in the stack
void newToken(char *name, int value, int constType) {
	StackTok *newTok = malloc(sizeof(StackTok));
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

// looking for type
TypeVar findType(char *_name) {
	if (strcmp(_name, "int") == 0)
		return intType;
	else if (strcmp(_name, "const") == 0)
		return constType;
	else
		return noneType;
}

// searches for matches in the name table
TokenType searchTN(char *name) {
	for (int i = 0; i < SIZETABLE; i++) {
		if (strcmp(name, TableNames[i].name) == 0) {
			return TableNames[i].type;
		}
	}
	return noneTok;
}

void enter() {
	enterTableNames("while", whileTok);
	enterTableNames("if", ifTok);
	enterTableNames("else", elseTok);
	enterTableNames("for", forTok);
	enterTableNames("const", typeTok);
	enterTableNames("int", typeTok);
	enterTableNames("print", printTok);
	enterTableNames("println", printlnTok);
	enterTableNames("input", inputTok);
}
