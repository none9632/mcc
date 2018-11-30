#include <stdlib.h>

#include "scan.h"
#include "table.h"

StackTok *top = NULL;
int _count = 0;

void enterTableNames(char *_name, TokenType _type) {
	strcpy(TableNames[_count].name, _name);
	TableNames[_count++].type = _type;
}

void newToken(char *_name, int _value, TokenType _tokenType, TypeVar _typeVar) {
	StackTok *newTok = (StackTok*)malloc(sizeof(StackTok));
	strcpy(newTok->name, _name);
	if (_value == 1)
		newTok->value = (int*)malloc(sizeof(int));
	else
		newTok->value = NULL;
	newTok->tokenType = _tokenType;
	newTok->typeVar = _typeVar;
	newTok->low = top;
	top = newTok;
}

StackTok* find(char *_name) {
	StackTok *tokObj = top;
	while (tokObj->tokenType != startTok) {
		if (strcmp(_name, tokObj->name) == 0) 
			break;
		tokObj = tokObj->low;
	}
	if (tokObj->tokenType == startTok) 
		error("undeclared variable name");
	return tokObj;
}

TypeVar findType(char *_name) {
	if (strcmp(_name, "int") == 0)
		return intType;
	else if (strcmp(_name, "double") == 0)
		return doubleType;
	else if (strcmp(_name, "char") == 0)
		return charType;
	else if (strcmp(_name, "bool") == 0)
		return boolType;
	else
		return noneType;
}

TokenType searchTN(char *_name) {
	for (int i = 0; i < SIZETABLE; i++) {
		if (strcmp(_name, TableNames[i].name) == 0) {
			return TableNames[i].type;
		}
	}
	return noneTok;
}

void enter(void) {
	enterTableNames("while", whileTok);
	enterTableNames("if", ifTok);
	enterTableNames("else", elseTok);
	enterTableNames("for", forTok);
	enterTableNames("const", typeTok);
	enterTableNames("int", typeTok);
	enterTableNames("double", typeTok);
	enterTableNames("char", typeTok);
	enterTableNames("bool", typeTok);
	enterTableNames("print", printTok);
	enterTableNames("println", printlnTok);
	enterTableNames("scan", scanTok);
}