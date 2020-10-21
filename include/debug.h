#ifndef TOY_DEBUG_H
#define TOY_DEBUG_H

#include "lexer.h"
#include "parser.h"
#include "literal.h"
#include "dictionary.h"
#include "scope.h"

void printToken(Token* token);
void printChunk(Chunk* chunk, char* prepend);
void printLiteralArray(LiteralArray* array, char* prepend);
void printDictionary(Dictionary* dictionary);

void printChunkByteArray(Chunk* chunk);
void dbPrintLiteral(Literal* literal, char* prepend, bool shrt);

void printScope(Scope* scope);
//TODO: integration testing

#endif