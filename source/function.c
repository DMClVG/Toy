#include "function.h"

#include "memory.h"

void initFunction(Function* func) {
	func->capacity = 0;
	func->count = 0;
	func->parameters = NULL;
	func->chunk = ALLOCATE(Chunk, 1);
	initChunk(func->chunk);
}

void freeFunction(Function* func) {
	FREE_ARRAY(int, func->parameters, func->capacity);
	freeChunk(func->chunk);
}

void pushFunctionParameter(Function* func, int index) {
	//expand the parameter count/capacity if needed
	if (func->count + 1 > func->capacity) {
		int oldCapacity = func->capacity;
		func->capacity = GROW_CAPACITY(oldCapacity);
		func->parameters = GROW_ARRAY(int, func->parameters, oldCapacity, func->capacity);
	}

	func->parameters[func->count++] = index;
}
