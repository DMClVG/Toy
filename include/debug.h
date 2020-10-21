#ifndef TOY_DEBUG_H
#define TOY_DEBUG_H

#include "lexer.h"
#include "parser.h"
#include "dictionary.h"

void printToken(Token* token);
void printChunk(Chunk* chunk, char* prepend);
void printLiteralArray(LiteralArray* array, char* prepend);
void printDictionary(Dictionary* dictionary);

void printChunkByteArray(Chunk* chunk);
void dbPrintLiteral(Literal* literal, char* prepend, bool shrt);

//TODO: integration testing

#endif