#include "compiler.h"
#include "scanner.h"
#include "common.h"

#include <stdio.h>

void compile(const char* source) {
	//TODO: return bytecode for running or saving to a file
	Scanner scanner;

	initScanner(&scanner, source);
	int line = -1;
	for (;;) {
		Token token = scanToken(&scanner);

		//nice debugging
		if (token.line != line) {
			line = token.line;
			printf("%4d ", line);
		} else {
			printf("   | ");
		}

		//print the token
		printf("%2d '%.*s'\n", token.type, token.length, token.start);

		if (token.type == TOKEN_EOF) break;
	}
}