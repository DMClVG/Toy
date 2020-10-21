#ifndef TOY_FUNCTION_H
#define TOY_FUNCTION_H

#include "common.h"

#include "scope.h"
#include "chunk.h"

typedef struct {
	LiteralArray parameters;
	Scope* scope; //the scope is not created until runtime
	Chunk* chunk;
} Function;

void initFunction(Function* func);
void freeFunction(Function* func);

Function* copyFunction(Function* orig);

#endif