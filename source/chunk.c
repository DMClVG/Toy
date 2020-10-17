#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
	chunk->capacity = 0;
	chunk->count = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	initLiteralArray(&chunk->literals);
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);

	freeLiteralArray(&chunk->literals);
	FREE(Chunk, chunk); //allocated in the parser
}

void writeChunk(Chunk* chunk, uint8_t val, int line) {
	//grow the arrays if necessary
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	//write a byte
	chunk->code[chunk->count] = val;
	chunk->lines[chunk->count] = line;
	chunk->count++;
}

void writeChunkLong(Chunk* chunk, uint32_t val, int line) {
	//grow the arrays if necessary
	if (chunk->capacity < chunk->count + sizeof(uint32_t)) {
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	//write a uint32
	*(uint32_t*)(chunk->code + chunk->count) = val;
	chunk->lines[chunk->count] = line;
	chunk->count += sizeof(uint32_t);
}
