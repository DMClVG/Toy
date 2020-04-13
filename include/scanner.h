#ifndef CTOY_SCANNER_H
#define CTOY_SCANNER_H

#include "token_type.h"

typedef struct {
	TokenType type;
	const char* start;
	int length;
	int line;
} Token;

void initScanner(const char* source);
Token scanToken();

#endif
