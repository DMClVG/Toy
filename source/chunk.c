#include "chunk.h"
#include "memory.h"

#include <stdlib.h>

void initChunk(Chunk* chunk) {
	chunk->capacity = 0;
	chunk->count = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	initValueArray(&chunk->constants);
	initTable(&chunk->strings);
	chunk->objects = NULL;
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	chunk->code[chunk->count] = byte;
	chunk->lines[chunk->count] = line;
	chunk->count++;
}

void writeChunkLong(Chunk* chunk, uint32_t val, int line) {
	if (chunk->capacity < chunk->count + 4) {
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	*(uint32_t*)(chunk->code + chunk->count) = val;
	chunk->lines[chunk->count] = line;
	chunk->count += 4;
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&chunk->constants);
	freeTable(&chunk->strings);
	freeObjectPool(&chunk->objects);
	initChunk(chunk);
}

//utilities
int pushConstant(Chunk* chunk, Value value) {
	writeValueArray(&(chunk->constants), value);
	return chunk->constants.count - 1;
}

void writeConstant(Chunk* chunk, Value value, int line) {
	//determine the correct opcode
	if (chunk->constants.count < 256) {
		//short type
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, pushConstant(chunk, value), line);
	} else {
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		writeChunkLong(chunk, pushConstant(chunk, value), line);
	}
}