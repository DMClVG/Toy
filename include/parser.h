#ifndef TOY_PARSER_H
#define TOY_PARSER_H

#include "lexer.h"
#include "chunk.h"

//DOCS: parsers are bound to a lexer, and turn the outputted tokens into chunks of data
typedef struct {
	Lexer* lexer;
	bool error; //I've had an error
	bool panic; //I am processing an error

	//track the last two outputs from the lexer
	Token current;
	Token previous;
} Parser;

void initParser(Parser* parser, Lexer* lexer);
void freeParser(Parser* parser);
Chunk* scanParser(Parser* parser);

#endif