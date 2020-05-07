#ifndef CTOY_PARSER_H
#define CTOY_PARSER_H

#include "scanner.h"
#include "chunk.h"
#include "object.h"
#include "common.h"

//local variables
typedef struct {
	Token name;
	int depth;
} Local;

//parser structure
typedef struct {
	//reference the scanner
	Scanner* scanner;

	//process the chunk
	Chunk* chunk;
	Token current;
	Token previous;

	//handle local vars/consts
	Local* locals;
	int localCapacity;
	int localCount;
	int scopeDepth;

	//error handling
	bool hadError;
	bool panicMode;
} Parser;

//initialize the parser
void initParser(Parser* parser, Scanner* scanner, Chunk* chunk);
void freeParser(Parser* parser);

#endif
