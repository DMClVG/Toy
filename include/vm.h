#ifndef CTOY_VM_H
#define CTOY_VM_H

#include "chunk.h"
#include "value.h"

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

typedef struct {
	//instructions
	Chunk* chunk;
	uint8_t* ip;

	//stack stuff
	int capacity;
	int count;
	Value* stack;
} VM;

void initVM(VM* vm);
void freeVM(VM* vm);
void pushVM(VM* vm, Value value);
Value popVM(VM* vm);

InterpretResult runVM(VM* vm);

#endif
