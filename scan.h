#ifndef SCAN_H
#define SCAN_H

#include <stdio.h>

#define NAMESIZE 32

extern FILE* file;
typedef enum {
	semiTok, commaTok, nameTok, numTok,
	LbraketTok, RbraketTok, multTok, divTok, moreTok,
	lessTok, moreEQTok, lessEQTok, equalTok, minusTok,
	plusTok, compTok, ifTok, whileTok, elseTok, 
	modTok, varTok, forTok, RbracesTok, LbracesTok
} TokenType;
extern TokenType tokenType;
extern int number;
extern char name[NAMESIZE];

void scanning(void);

#endif