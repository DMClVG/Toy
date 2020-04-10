#ifndef CTOY_CHUNK_H
#define CTOY_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
	OP_RETURN,
	OP_CONSTANT,
	OP_CONSTANT_LONG, //for more than 256 constants
} OpCode;

typedef struct {
	int capacity;
	int count;
	uint8_t* code;
	int* lines;
	ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void writeChunkLong(Chunk* chunk, uint32_t val, int line);
void freeChunk(Chunk* chunk);

//utilities
int pushConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line);

#endif
