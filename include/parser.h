#ifndef CTOY_PARSER_H
#define CTOY_PARSER_H

#include "scanner.h"
#include "chunk.h"
#include "common.h"

//parser rules
typedef struct {
	Scanner* scanner;
	Chunk* chunk;
	Token current;
	Token previous;
	bool hadError;
	bool panicMode;
} Parser;

//parser rules
typedef void (*ParseFn)();

typedef enum { //TODO: add Toy's extra precedences
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_PRIMARY,
} Precedence;

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	//TODO: mixfix for ternary operator?
	Precedence precedence;
} ParseRule;

ParseRule* getRule(TokenType type);

//initialize the parser
void initParser(Parser* parser, Scanner* scanner, Chunk* chunk);

//compiler.c utility functions
void errorAt(Parser* parser, Token* token, const char* message);
void errorAtPrevious(Parser* parser, const char* message);
void errorAtCurrent(Parser* parser, const char* message);

void advance(Parser* parser);
void consume(Parser* parser, TokenType type, const char* message);

//parser.c functions
void emitByte(Parser* parser, uint8_t byte);
void emitBytes(Parser* parser, uint8_t byte1, uint8_t byte2);
void emitConstant(Parser* parser, Value value);

void expression(Parser* parser);

#endif
