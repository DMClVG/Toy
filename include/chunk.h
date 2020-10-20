#ifndef TOY_CHUNK_H
#define TOY_CHUNK_H

#include "common.h"
#include "literal.h"

//DOCS: chunks are the intermediaries between parsers and compilers
typedef struct {
	int capacity; //how much space is allocated for the code
	int count; //the current index of the code
	uint8_t* code; //the opcodes + instructions
	int* lines; //for error messages

	LiteralArray literals; //an array of literal values
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t val, int line);
void writeChunkLong(Chunk* chunk, uint32_t val, int line);

Chunk* copyChunk(Chunk* chunk);

#endif