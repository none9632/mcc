#ifndef SCAN_H
#define SCAN_H

#include "table.h"

extern TokenType tokenType;
extern int nValue;
extern char name[NAMESIZE];
extern char CH, bufferCH;
extern int posLine, posSym;

void getNextCH();
void nextTok();

#endif
