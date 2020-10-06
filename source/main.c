//DOCS: This file is regularly re-written as testing continues
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

//for testing
#include "debug.h"

//read a file and return it as a char array
char* readFile(const char* path) {
	FILE* file = fopen(path, "rb");

	if (file == NULL) {
		fprintf(stderr, "Could not open file \"%s\"\n", path);
		exit(74);
	}

	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(fileSize + 1);

	if (buffer == NULL) {
		fprintf(stderr, "Not enough memory to read \"%s\"\n", path);
		exit(74);
	}

	size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

	if (bytesRead < fileSize) {
		fprintf(stderr, "Could not read file \"%s\"\n", path);
		exit(74);
	}

	fclose(file);

	buffer[bytesRead] = '\0';

	return buffer;
}

//entry point
int main(int argc, const char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s [file]\n", argv[0]);
		return 0;
	}

	char* source = readFile(argv[1]);

	Lexer lexer;

	initLexer(&lexer, source);

	Token token;
	do {
		token = scanLexer(&lexer);

		printToken(&token);

	} while (token.type != TOKEN_EOF);

	free((void*)source);

	return 0;
}