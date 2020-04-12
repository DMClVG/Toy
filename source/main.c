#include "common.h"
#include "debug.h"
#include "vm.h"
#include "chunk.h"

#include <stdio.h>

int main(int argc, const char *argv[]) {
	//build the chunk
	Chunk chunk;
	initChunk(&chunk);

/*
	for (int line = 0; line < 500; line++) {
		writeConstant(&chunk, line, line);
	}

	writeChunk(&chunk, OP_NEGATE, 500);
	writeChunk(&chunk, OP_RETURN, 500);

*/
	writeConstant(&chunk, 2.0, 1);
	writeConstant(&chunk, 3.0, 1);
	writeChunk(&chunk, OP_ADD, 1); //produces 5
	writeConstant(&chunk, 2.5, 1);
	writeChunk(&chunk, OP_DIVIDE, 1); //produces 2
//*/
	disassembleChunk(&chunk, "test chunk");

	//build the VM
	VM vm;
	initVM(&vm);

	vm.chunk = &chunk;
	vm.ip = chunk.code;

	runVM(&vm);

	freeVM(&vm);
	freeChunk(&chunk);
	return 0;
}