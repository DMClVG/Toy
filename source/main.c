#include "common.h"
#include "debug.h"
#include "vm.h"
#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

void repl() {
	VM vm;
	initVM(&vm);

	char input[1024];

	for (;;) {
		printf(">");

		if (!fgets(input, sizeof(input), stdin)) {
			printf("\n");
			break;
		}

		if (interpretVM(&vm, input) != INTERPRET_OK) {
			cleanVM(&vm);
		}
	}

	freeVM(&vm);
}

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

void runFile(const char* path) {
	VM vm;
	initVM(&vm);

	char* source = readFile(path);
	InterpretResult result = interpretVM(&vm, source);

	free(source);
	freeVM(&vm);

	if (result == INTERPRET_COMPILE_ERROR) exit(65);
	if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

//entry point
int main(int argc, const char *argv[]) {
	switch(argc) {
		case 1:
			repl();
			break;

		case 2:
			runFile(argv[1]);
			break;

		default:
			fprintf(stderr, "Usage: %s [path]\n", argv[0]);
			exit(64);
	}

	return 0;
}