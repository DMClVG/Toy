#ifndef CTOY_DEBUG_H
#define CTOY_DEBUG_H

#include "chunk.h"
#include "object.h"

void disassembleChunk(Chunk* chunk, char* name);
int disassembleInstruction(Chunk* chunk, int offset);

void disassembleObjectPool(Object* pool, char* name);

#endif
