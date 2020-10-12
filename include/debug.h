#ifndef TOY_DEBUG_H
#define TOY_DEBUG_H

#include "lexer.h"
#include "parser.h"
#include "table.h"

void printToken(Token* token);
void printChunk(Chunk* chunk);
void printTable(Table* table);

void printChunkByteArray(Chunk* chunk);
void dbPrintLiteral(Literal* literal);

//TODO: integration testing

#endif