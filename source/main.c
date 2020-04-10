#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
	Chunk chunk;
	initChunk(&chunk);

	for (int line = 0; line < 500; line++) {
		writeConstant(&chunk, 1.2, line);
	}
	writeChunk(&chunk, OP_RETURN, 500);

	disassembleChunk(&chunk, "test chunk");

	freeChunk(&chunk);
	return 0;
}