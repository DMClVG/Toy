#include "function.h"

#include "memory.h"

void initFunction(Function* func) {
	initLiteralArray(&func->parameters);
	func->chunk = ALLOCATE(Chunk, 1);
	initChunk(func->chunk);
	func->scope = NULL;
}

void freeFunction(Function* func) {
	freeLiteralArray(&func->parameters);
	//DON'T free scope
	freeChunk(func->chunk);
}
