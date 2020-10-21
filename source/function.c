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

	if (func->scope) {
		func->scope = unreferenceScope(func->scope);
	}

	freeChunk(func->chunk);
	FREE(Function, func);
}

Function* copyFunction(Function* orig) {
	Function* func = ALLOCATE(Function, 1);
	initLiteralArray(&func->parameters);

	func->scope = referenceScope(orig->scope);

	//copy manually
	for (int i = 0; i < orig->parameters.count; i++) {
		writeLiteralArray(&func->parameters, orig->parameters.literals[i]);
	}

	//copy the chunk automagically
	func->chunk = copyChunk(orig->chunk);

	return func;
}