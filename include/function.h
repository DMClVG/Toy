#ifndef TOY_FUNCTION_H
#define TOY_FUNCTION_H

#include "common.h"

#include "chunk.h"

typedef struct {
	int capacity;
	int count;
	int* parameters; //indexes the OUTER chunk's literals
	//TODO: (1) prevent lost scopes getting deleted
	Chunk* chunk;
} Function;

void initFunction(Function* func);
void freeFunction(Function* func);

void pushFunctionParameter(Function* func, int index); //this will push the INDEX of the literal, stored in the outer chunk

#endif