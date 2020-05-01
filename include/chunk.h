#ifndef CTOY_CHUNK_H
#define CTOY_CHUNK_H

#include "common.h"
#include "value.h"
#include "object.h"
#include "table.h"

typedef enum {
	OP_RETURN,
	OP_CONSTANT,
	OP_CONSTANT_LONG, //for more than 256 constants
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_NOT,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
} OpCode;

typedef struct {
	int capacity;
	int count;
	uint8_t* code;
	int* lines;
	ValueArray constants;
	Table strings; //track the strings used in this chunk
	Object* objects; //track the memory generated by this chunk
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
void writeChunkLong(Chunk* chunk, uint32_t val, int line);
void freeChunk(Chunk* chunk);

//utilities
int pushConstant(Chunk* chunk, Value value);
void writeConstant(Chunk* chunk, Value value, int line);

#endif
