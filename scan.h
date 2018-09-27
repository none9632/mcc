#ifndef SCAN_H
#define SCAN_H

#include <stdio.h>
#include <ctype.h>

FILE* file;
enum tokens {
	semiTok, commaTok, nameTok, numTok,
	LbraketTok, RbraketTok, multTok, divTok, moreTok,
	lessTok, moreEQTok, lessEQTok, equalTok, minusTok,
	plusTok, compTok
};

void scanning(void);

#endif