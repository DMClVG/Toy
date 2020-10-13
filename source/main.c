//DOCS: This file is regularly re-written as testing continues
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "toy.h"

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

//run functions
void runFile(const char* fname) {
	char* source = readFile(fname);

	Lexer lexer;
	Parser parser;
	Toy toy;

	initLexer(&lexer, source);
	initParser(&parser, &lexer);
	initToy(&toy);

	Chunk* chunk = scanParser(&parser);

	executeChunk(&toy, chunk);

	freeChunk(chunk);

	freeToy(&toy);
	freeParser(&parser);
	free((void*)source);
}

void repl() {
	const int size = 2048;
	char input[size];
	memset(input, 0, size);

	Parser parser;
	Toy toy;

	initToy(&toy);

	for(;;) {
		printf(">");
		fgets(input, size, stdin);

		//setup
		Lexer lexer;

		initLexer(&lexer, input);
		initParser(&parser, &lexer);

		//run
		Chunk* chunk = scanParser(&parser);

		//clean up the memory
		if (parser.error) {
			freeChunk(chunk);
			freeParser(&parser);
			break;
		}

		executeChunk(&toy, chunk);
		freeChunk(chunk);

		//cleanup
		freeParser(&parser);
	}

	freeToy(&toy);
}

//entry point
int main(int argc, const char* argv[]) {
	if (argc == 1) {
		repl();
	} else if (argc == 2) {
		runFile(argv[1]);
	} else {
		printf("Usage: %s [file]\n", argv[0]);
	}

	return 0;
}