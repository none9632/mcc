#ifndef SCAN_H
#define SCAN_H

#include "table.h"

extern TokenType tokenType;
extern int value;
extern char name[NAMESIZE];
extern char CH, bufferCH;
extern int posLine, posSym;

void getNextCH(void);
void nextTok(void);	

#endif