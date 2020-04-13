#ifndef CTOY_SCANNER_H
#define CTOY_SCANNER_H

#include "token_type.h"

typedef struct {
	TokenType type;
	const char* start;
	int length;
	int line;
} Token;

typedef struct {
	const char* start;
	const char* current;
	int line;
} Scanner;

void initScanner(Scanner* scanner, const char* source);
Token scanToken(Scanner* scanner);

#endif
