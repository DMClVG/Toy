#ifndef TOY_TOY_H
#define TOY_TOY_H

#include "common.h"
#include "literal.h"
#include "chunk.h"
#include "dictionary.h"

typedef struct {
	bool error; //I've had a runtime error
	bool panic; //I'm processing a runtime error

	int capacity; //capacity of the stack
	int count; //number of literals in the stack
	Literal** stack; //raw literal array for the stack

	uint8_t* pc; //program counter

	LiteralArray garbage; //can be cleaned between "declarations"

	Dictionary constants; //a dict of live constants
	Dictionary variables; //a dict of live variables
	//TODO: scope via an array of tables?
} Toy;

void initToy(Toy* toy);
void freeToy(Toy* toy);
void executeChunk(Toy* toy, Chunk* chunk); //pass one chunk at a time

#define PUSH_TEMP_LITERAL(TOY, TEMP_LITERAL) \
	writeLiteralArray(&TOY->garbage, TEMP_LITERAL); \
	pushLiteral(TOY, &TOY->garbage.literals[TOY->garbage.count - 1]);

#endif