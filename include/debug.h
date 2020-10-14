#ifndef TOY_DEBUG_H
#define TOY_DEBUG_H

#include "lexer.h"
#include "parser.h"
#include "dictionary.h"

void printToken(Token* token);
void printChunk(Chunk* chunk);
void printLiteralArray(LiteralArray* literalArray);
void printDictionary(Dictionary* dictionary);

void printChunkByteArray(Chunk* chunk);
void dbPrintLiteral(Literal* literal);

//TODO: integration testing

#endif