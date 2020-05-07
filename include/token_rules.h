#ifndef CTOY_TOKEN_RULES_H
#define CTOY_TOKEN_RULES_H

#include "token_type.h"
#include "parser.h"

//TODO: add Toy's extra precedences
typedef enum {
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

//for the pratt table
typedef void (*ParseFn)(Parser* parser, bool canAssign);

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	//TODO: mixfix for ternary operator?
	Precedence precedence;
} ParseRule;

//get the rule for each token type
ParseRule* getRule(TokenType type);

void parsePrecendence(Parser* parser, Precedence precedence);

#endif
