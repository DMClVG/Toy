#ifndef CTOY_COMPILER_H
#define CTOY_COMPILER_H

#include "chunk.h"

//compile source code into a Chunk
bool compile(const char* source, Chunk* chunk);

#endif
