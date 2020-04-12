#ifndef CTOY_DEBUG_H
#define CTOY_DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk* chunk, char* name);
int disassembleInstruction(Chunk* chunk, int offset);

#define DEBUG_TRACE_EXECUTION

#endif
