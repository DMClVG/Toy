#include "common.h"
#include "debug.h"
#include "vm.h"
#include "chunk.h"

int main(int argc, const char *argv[]) {
	initVM();

	//build the chunk
	Chunk chunk;
	initChunk(&chunk);

/*
	for (int line = 0; line < 10; line++) {
		writeConstant(&chunk, line, line);
	}

	writeChunk(&chunk, OP_NEGATE, 10);

	for (int line = 0; line < 10; line++) {
		writeChunk(&chunk, OP_RETURN, 500);
	}
*/

	writeConstant(&chunk, 2.0, 1);
	writeConstant(&chunk, 3.0, 1);
	writeChunk(&chunk, OP_ADD, 1); //produces 5
	writeConstant(&chunk, 2.5, 1);
	writeChunk(&chunk, OP_DIVIDE, 1); //produces 2

//	disassembleChunk(&chunk, "test chunk");
	interpret(&chunk);

	freeChunk(&chunk);
	freeVM();
	return 0;
}