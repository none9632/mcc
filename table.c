#include <stdlib.h>

#include "scan.h"
#include "table.h"

StackTok *top = NULL;
int count = 0;

void enterTableNames(char *_name, TokenType _type) {
	strcpy(TableNames[count].name, _name);
	TableNames[count++].type = _type;
}

void newToken(char *_name, int _value, TokenType _tokenType, TypeVar _typeVar) {
	StackTok *newTok = (StackTok*)malloc(sizeof(*newTok));
	strcpy(newTok->name, _name);
	newTok->value = _value;
	newTok->tokenType = _tokenType;
	newTok->typeVar = _typeVar;
	newTok->low = top;
	top = newTok;
}

StackTok* find(char *_name) {
	StackTok *tokObj = top;
	while (tokObj->tokenType != startTok) {
		if (tokObj->name == _name) 
			break;
		tokObj = tokObj->low;
	}
	if (tokObj->tokenType == startTok)
		error("undeclared variable name");
	return tokObj;
}

TypeVar findType(char *_name) {
	if (*_name == "int")
		return intType;
	else if (*_name == "double")
		return doubleType;
	else if (*_name == "char")
		return charType;
	else
		return noneType;
}

int searchTN(char *_name) {
	for (int i = 0; i < SIZETABLE; i++) {
		if (TableNames[i].name == _name) 
			tokenType = TableNames[i].type;
	}
	return 0;
}

void enter(void) {
	enterTableNames("while", whileTok);
	enterTableNames("if", ifTok);
	enterTableNames("else", elseTok);
	enterTableNames("for", forTok);
	enterTableNames("int", typeTok);
	enterTableNames("double", typeTok);
	enterTableNames("char", typeTok);
}