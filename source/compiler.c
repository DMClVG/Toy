#include "compiler.h"
#include "scanner.h"
#include "common.h"

#include <stdio.h>

void compile(const char* source) {
	initScanner(source);
	int line = -1;
	for (;;) {
		Token token = scanToken();

		//nice debugging
		if (token.line != line) {
			line = token.line;
			printf("%4d ", line);
		} else {
			printf("   | ");
		}

		//print the token
		printf("%2d '%*.s'\n", token.type, token.length, token.start);

		if (token.type == TOKEN_EOF) break;
	}
}