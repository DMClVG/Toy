#ifndef TOY_PARSER_H
#define TOY_PARSER_H

#include "common.h"
#include "lexer.h"
#include "table.h"
#include "literal.h"

//DOCS: parsers are bound to a lexer, and turn the outputted tokens into chunks of data
typedef struct {
	Lexer* lexer;
} Parser;

//DOCS: chunks are the intermediaries between parsers and compilers
typedef struct {
	int capacity; //how much space is allocated for the code
	int count; //the current index of the code
	uint8_t* code; //the opcodes + instructions

	LiteralArray literals; //an array of literal values
	Table variables; //a table of variables
} Chunk;

void initParser(Parser* parser, Lexer* lexer);
void freeParser(Parser* parser);

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t val, int line);
void writeChunkLong(Chunk* chunk, uint32_t val, int line);

//void pushLiteral(Chunk* chunk, );

#endif