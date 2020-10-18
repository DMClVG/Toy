#ifndef TOY_TOY_H
#define TOY_TOY_H

#include "common.h"
#include "literal.h"
#include "chunk.h"
#include "scope.h"

typedef struct {
	bool error; //I've had a runtime error
	bool panic; //I'm processing a runtime error

	int capacity; //capacity of the stack
	int count; //number of literals in the stack
	int* indexes; //array members refer to index within the garbage array

	uint8_t* pc; //program counter

	LiteralArray garbage; //can be cleaned between "declarations"

	Scope* scope;
} Toy;

void initToy(Toy* toy);
void freeToy(Toy* toy);
void executeChunk(Toy* toy, Chunk* chunk); //pass one chunk at a time

#endif