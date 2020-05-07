#ifndef CTOY_PARSER_TOOLS_H
#define CTOY_PARSER_TOOLS_H

#include "parser.h"

//error handling
void errorAt(Parser* parser, Token* token, const char* message);
void errorAtPrevious(Parser* parser, const char* message);
void errorAtCurrent(Parser* parser, const char* message);
void synchronize(Parser* parser);

void advance(Parser* parser);
void consume(Parser* parser, TokenType type, const char* message);
bool match(Parser* parser, TokenType type);
bool check(Parser* parser, TokenType type);

//conveniences
void emitByte(Parser* parser, uint8_t byte);
void emitBytes(Parser* parser, uint8_t byte1, uint8_t byte2);
void emitLong(Parser* parser, uint32_t lng);
uint32_t emitConstant(Parser* parser, Value value);
uint32_t idenifierConstant(Parser* parser, Token* name);
bool identifiersEqual(Token* a, Token* b);

//scope
void markInitialized(Parser* parser);
void beginScope(Parser* parser);
void endScope(Parser* parser);

//vars
void declareVariable(Parser* parser);
void defineVariable(Parser* parser, uint32_t global);
uint32_t parseVariable(Parser* parser, const char* errorMsg);
void addLocal(Parser* parser, Token name);
uint32_t resolveLocal(Parser* parser, Token* name);
void namedVariable(Parser* parser, Token name, bool canAssign);

#endif