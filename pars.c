#include <string.h>

#include "pars.h"
#include "table.h"
#include "scan.h"
#include "error.h"

void checkTok(TokenType _tokenType) {
	if (tokenType == _tokenType)
		error("syntax error");
	nextTok();
}

TypeVar factor(void) {
	if (tokenType == numTok) 
		return intType;
	else if (tokenType == nameTok) 
		return find(name)->typeVar;
	else if (tokenType == LbraketTok)
		return expr();
	else
		error("syntax error");
}

TypeVar term(void) {
	TypeVar typeVar = factor();
	return typeVar;
}

TypeVar sum(void) {
	TypeVar typeVar = test();
	return typeVar;
}

TypeVar module(void){
	TypeVar typeVar = test();
	return typeVar;
}

TypeVar test(void) {
	TypeVar typeVar = test();
	return typeVar;
}

void var(void) {
	TypeVar typeVar = findType(name);
	if (typeVar != noneType) 
		newToken(name, value, tokenType, typeVar);
	else 
		error("unknown type");
}

int expr(void) { 
	TypeVar typeVar = test();
	if (tokenType == LbraketTok) {
		checkTok(LbraketTok);
		expr();
		checkTok(RbraketTok);
	}
	else if (tokenType == typeTok) 
		var();
	else if () // исправить
		test();
	else 
		error("syntax error");
	return 0;
}

void initWhile(void) {
	checkTok(whileTok);
	checkTok(LbraketTok);
	expr();
	checkTok(RbraketTok);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
}

void initIf(void) {
	checkTok(ifTok);
	checkTok(LbraketTok);
	expr();
	checkTok(RbraketTok);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
}

void statement(void) {
	if (tokenType == nameTok) {
		expr();
		checkTok(commaTok);
	}
	else if (tokenType == whileTok) 
		initWhile();
	else if (tokenType == ifTok) 
		initIf();
}

void parsing(void) {
	newToken("", 0, 0, startTok);
	checkTok(LbracesTok);
	statement();
	checkTok(RbracesTok);
}