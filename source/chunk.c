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

Chunk* copyChunk(Chunk* chunk) {
	Chunk* ret = ALLOCATE(Chunk, 1);
	initChunk(ret);

	ret->capacity = chunk->capacity;
	ret->count = chunk->count;

	ret->code = ALLOCATE(uint8_t, ret->capacity);
	memcpy(ret->code, chunk->code, ret->capacity);
	ret->lines = ALLOCATE(int, ret->capacity);
	memcpy(ret->lines, chunk->lines, ret->capacity);

	//write the literal array manually
	for (int i = 0; i < chunk->literals.count; i++) {
		writeLiteralArray(&ret->literals, chunk->literals.literals[i]);
	}

	return ret;
}