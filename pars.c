#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "lib.h"
#include "table.h"
#include "lex.h"
#include "error.h"
#include "gen.h"
#include "launch.h"
#include "pars.h"

int minus = 0;

void checkTok(int _tokenType) {
	if (tokenType != _tokenType)
		error("syntax error", 1);
	nextTok();
}

TypeVar term() {
	if (tokenType == MINUS_TOK || tokenType == PLUS_TOK) {
		if (tokenType == MINUS_TOK)
			minus = 1;
		nextTok();
	}
	if (tokenType == NUM_TOK) {
		if (minus == 1) {
			nValue = -nValue;
			minus = 0;
		}
		int *bufferPoint = malloc(sizeof(int));
		*bufferPoint = nValue;
		gen(CNONE, bufferPoint);
		nextTok();
		return intType;
	}
	else if (tokenType == IDENT_TOK) {
		StackTok *nameVar = find(name);
		if (nameVar->value == NULL)
			error("syntax error", 1);
		if (minus == 1) {
			gen(CNEGP, nameVar->value);
			minus = 0;
		}
		else
			gen(CNONEP, nameVar->value);
		nextTok();
		return intType;
	}
	else if (tokenType == LBRAKET_TOK) {
		int bufferMinus = 0;
		nextTok();
		if (minus == 1) {
			bufferMinus = 1;
			minus = 0;
		}
		TypeVar typeVar = expr();
		checkTok(RBRAKET_TOK);
		if (bufferMinus == 1) {
			gen(CNEG, NULL);
		}
		return typeVar;
	}
	return noneType;
}

TypeVar factor() {
	TypeVar typeVar = term();
	TokenType op;
	if (tokenType == MULT_TOK || tokenType == DIV_TOK) {
		do {
			op = tokenType;
			nextTok();
			typeVar = term();
			switch (op) {
			case MULT_TOK: gen(CMULT, NULL); break;
			case DIV_TOK: gen(CDIV, NULL); break;
			}
		} while (tokenType == MULT_TOK || tokenType == DIV_TOK);
	}
	return typeVar;
}

TypeVar addend() {
	TypeVar typeVar = factor();
	TokenType op;
	if (tokenType == PLUS_TOK || tokenType == MINUS_TOK) {
		do {
			op = tokenType;
			nextTok();
			typeVar = factor();
			switch (op) {
			case PLUS_TOK: gen(CPLUS, NULL); break;
			case MINUS_TOK: gen(CMINUS, NULL); break;
			}
		} while (tokenType == PLUS_TOK || tokenType == MINUS_TOK);
	}
	return typeVar;
}

TypeVar module(){
	TypeVar typeVar = addend();
	TokenType op;
	if (typeVar == intType) {
		if (tokenType == MOD_TOK) {
			do {
				op = tokenType;
				nextTok();
				typeVar = addend();
				if (typeVar != intType)
					error("syntax error", 1);
				gen(CMOD, NULL);
			} while (tokenType == MOD_TOK);
		}
	}
	return typeVar;
}

TypeVar test() {
	TypeVar typeVar = module();
	TokenType op;
	if (tokenType == EQUAL_TOK || tokenType == MORE_TOK || tokenType == MOREEQ_TOK ||
		tokenType == LESS_TOK || tokenType == LESSEQ_TOK || tokenType == NOTEQ_TOK) {
		op = tokenType;
		nextTok();
		typeVar = module();
		switch (op) {
		case EQUAL_TOK: gen(CEQUAL, NULL); break;
		case MORE_TOK: gen(CMORE, NULL); break;
		case MOREEQ_TOK: gen(CMOREEQ, NULL); break;
		case LESS_TOK: gen(CLESS, NULL); break;
		case LESSEQ_TOK: gen(CLESSEQ, NULL); break;
		case NOTEQ_TOK: gen(CNOTEQ, NULL); break;
		}
		return boolType;
	}
	return typeVar;
}

TypeVar expr() {
	TypeVar typeVar = test();
	if (typeVar == noneType)
		error("syntax error", 1);
	return typeVar;
}

// assignment function
void assing() {
	StackTok *nameVar = find(name);
	TokenType bufferTT;
	if (nameVar->constType == 1)
		error("syntax error", 1);
	nextTok();
	bufferTT = tokenType;
	nextTok();
	expr();
	if (nameVar->value == NULL) {
		if (tokenType == ASSIGN_TOK)
			nameVar->value = malloc(sizeof(int));
		else
			error("syntax error", 1);
	}
	gen(CLOAD, nameVar->value);
	switch (bufferTT) {
	case PLUSA_TOK: gen(CPLUSA, NULL); break;
	case MINUSA_TOK: gen(CMINUSA, NULL); break;
	case MULTA_TOK: gen(CMULTA, NULL); break;
	case DIVA_TOK: gen(CDIVA, NULL); break;
	case MODA_TOK: gen(CMODA, NULL); break;
	case ASSIGN_TOK: gen(CASSIGN, NULL); break;
	default: error("syntax error", 1); break;
	}
	checkTok(SEMI_TOK);
}

// initialization variable
void initVar() {
	TypeVar typeVar = findType(name);
	int buffer;
	if (typeVar == constType) {
		buffer = 1;
		nextTok();
		typeVar = findType(name);
	}
	if (typeVar != noneType && typeVar != constType) {
		do {
			nextTok();
			checkTok(IDENT_TOK);
			if (tokenType == ASSIGN_TOK) {
				char *bufferName = malloc(sizeof(char) * 32);
				strcpy(bufferName, name);
				nextTok();
				if (typeVar != expr())
					error("syntax error", 1);
				newToken(bufferName, 1, buffer);
				gen(CLOAD, find(bufferName)->value);
				gen(CASSIGN, NULL);
			}
			else
				newToken(name, 0, buffer);
		} while (tokenType == COMMA_TOK);
	}
	else
		error("unknown type", 1);
	checkToks(SEMI_TOK);
}


// initialization while
void initWhile() {
	int *point = malloc(sizeof(int)),
		*bufferPoint = malloc(sizeof(int));
	nextTok();
	checkTok(LBRAKET_TOK);
	if (expr() != boolType)
		error("syntax error", 1);
	checkTok(RBRAKET_TOK);
	*bufferPoint = cGen - 1;
	gen(CWHILE, bufferPoint);
	gen(CJUMP, point);
	checkTok(LBRACES_TOK);
	statement();
	checkTok(RBRACES_TOK);
	*point = cGen;
	gen(CSTOP, NULL);
}

// initialization if
void initIf() {
	int *point1 = malloc(sizeof(int)),
		*point2 = malloc(sizeof(int));
	nextTok();
	checkTok(LBRAKET_TOK);
	if (expr() != boolType)
		error("syntax error", 1);
	checkTok(RBRAKET_TOK);
	gen(CIF, NULL);
	gen(CJUMP, point1);
	checkTok(LBRACES_TOK);
	statement();
	checkTok(RBRACES_TOK);
	*point1 = cGen - 1;
	gen(CSTOP, NULL);
	if (tokenType == ELSE_TOK) {
		gen(CELSE, NULL);
		gen(CJUMP, point2);
		nextTok();
		checkTok(LBRACES_TOK);
		statement();
		checkTok(RBRACES_TOK);
		*point2 = cGen - 1;
		gen(CSTOP, NULL);
	}
}

// output function initialization
void initPrint() {
	TokenType bufferTT = tokenType;
	nextTok();
	checkTok(LBRAKET_TOK);
	if (tokenType == IDENT_TOK) {
		gen(CPRINT, find(name)->value);
		nextTok();
	}
	else if (tokenType == NUM_TOK) {
		if (expr() != intType)
			error("syntax error", 1);
		gen(CPRINTN, NULL);
	}
	else if (tokenType == DOBLQUOT_TOK) {
		int *text = malloc(sizeof(char) * 1024);
		int i = 0;
		while (CH != '\"' && CH != EOF) {
			if (i >= 1024)
				error("big the text", 1);
			text[i++] = CH;
			getNextCH();
		}
		if (CH == EOF)
			error("syntax error", 1);
		text[i] = '\0';
		getNextCH();
		gen(CPRINTS, text);
		nextTok();
	}
	else
		error("syntax error", 1);
	if (bufferTT == PRINTLN_TOK)
		gen(CPRINTLN, NULL);
	checkTok(RBRAKET_TOK);
	checkTok(SEMI_TOK);
}

// initialization of the input function
void initInput() {
	nextTok();
	checkTok(LBRAKET_TOK);
	if (tokenType != IDENT_TOK)
		error("syntax error", 1);
	StackTok *nameVar = find(name);
	if (nameVar->value == NULL)
		nameVar->value = malloc(sizeof(int));
	gen(CINPUT, nameVar->value);
	nextTok();
	checkTok(RBRAKET_TOK);
	checkTok(SEMI_TOK);
}

void statement() {
	while (tokenType != RBRACES_TOK && tokenType != EOF_TOK) {
		if (tokenType == IDENT_TOK)
			assing();
		else if (tokenType == TYPE_TOK)
			initVar();
		else if (tokenType == WHILE_TOK)
			initWhile();
		else if (tokenType == IF_TOK)
			initIf();
		else if (tokenType == PRINT_TOK || tokenType == PRINTLN_TOK)
			initPrint();
		else if (tokenType == INPUT_TOK)
			initInput();
	}
}

void parsing(Vector *tokens) {
	newToken("", 0, 2);
	checkTok(LBRACES_TOK);
	statement();
	checkTok(RBRACES_TOK);
	gen(CSTOP, NULL);
}
