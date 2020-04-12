#ifndef CTOY_VM_H
#define CTOY_VM_H

#include "chunk.h"
#include "value.h"

//TODO: dynamic stack size
#define STACK_MAX 1024

typedef struct {
	Chunk* chunk;
	uint8_t* ip;
	Value stack[STACK_MAX];
	Value* stackTop;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM();
void freeVM();
void push(Value value);
Value pop();

InterpretResult interpret(Chunk* chunk);

#endif
