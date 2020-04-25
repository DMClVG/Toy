#include "parser.h"

#include <stdlib.h>

//forward declarations
ParseRule* getRule(TokenType type);

//exposed functions
void initParser(Parser* parser, Scanner* scanner, Chunk* chunk) {
	parser->scanner = scanner;
	parser->chunk = chunk;
	//parser->previous = ???
	//parser->current = ???
	parser->hadError = false;
	parser->panicMode = false;
}

//convenience
void emitByte(Parser* parser, uint8_t byte) {
	writeChunk(parser->chunk, byte, parser->previous.line);
}

void emitBytes(Parser* parser, uint8_t byte1, uint8_t byte2) {
	emitByte(parser, byte1);
	emitByte(parser, byte2);
}

void emitConstant(Parser* parser, Value value) {
	writeConstant(parser->chunk, value, parser->previous.line);
}

//precedence
static void parsePrecendence(Parser* parser, Precedence precedence) {
	advance(parser);

	ParseFn prefixRule = getRule(parser->previous.type)->prefix;
	if (prefixRule == NULL) {
		errorAtCurrent(parser, "Expected expression");
		return;
	}

	prefixRule(parser);

	while (precedence <= getRule(parser->current.type)->precedence) {
		advance(parser);
		ParseFn infixRule = getRule(parser->previous.type)->infix;
		infixRule(parser);
	}
}

//expressions
static void number(Parser* parser) {
	double value = strtod(parser->previous.start, NULL);
	emitConstant(parser, value);
}

static void grouping(Parser* parser) {
	expression(parser);
	consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after expression");
}

static void unary(Parser* parser) {
	TokenType operatorType = parser->previous.type;

	parsePrecendence(parser, PREC_UNARY);

	switch(operatorType) {
		case TOKEN_MINUS:
			emitByte(parser, OP_NEGATE);
			break;
		default:
			return;
	}
}

static void binary(Parser* parser) {
	TokenType operatorType = parser->previous.type;

	ParseRule* rule = getRule(operatorType);
	parsePrecendence(parser, (Precedence)(rule->precedence + 1));

	switch(operatorType) {
		case TOKEN_PLUS:
			emitByte(parser, OP_ADD);
			break;

		case TOKEN_MINUS:
			emitByte(parser, OP_SUBTRACT);
			break;

		case TOKEN_STAR:
			emitByte(parser, OP_MULTIPLY);
			break;

		case TOKEN_SLASH:
			emitByte(parser, OP_DIVIDE);
			break;

		default:
			return;
	}
}

void expression(Parser* parser) {
	parsePrecendence(parser, PREC_ASSIGNMENT);
}

//a pratt table
ParseRule parseRules[] = {
	//single character tokens
	{grouping,		NULL,			PREC_NONE}, // TOKEN_LEFT_PAREN,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_RIGHT_PAREN,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_LEFT_BRACE,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_RIGHT_BRACE,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_LEFT_BRACKET,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_RIGHT_BRACKET,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_SEMICOLON,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_COMMA,

	//one or two character tokens
	{NULL,			binary,			PREC_TERM}, // TOKEN_PLUS,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_PLUS_EQUAL,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_PLUS_PLUS,

	{unary,			binary,			PREC_TERM}, // TOKEN_MINUS,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_MINUS_EQUAL,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_MINUS_MINUS,
	//TODO: munus greater for objects

	{NULL,			binary,			PREC_FACTOR}, // TOKEN_STAR,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_STAR_EQUAL,

	{NULL,			binary,			PREC_FACTOR}, // TOKEN_SLASH,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_SLASH_EQUAL,

	{NULL,			NULL,			PREC_NONE}, // TOKEN_MODULO,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_MODULO_EQUAL,

	{NULL,			NULL,			PREC_NONE}, // TOKEN_BANG,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_BANG_EQUAL,

	{NULL,			NULL,			PREC_NONE}, // TOKEN_EQUAL,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_EQUAL_EQUAL,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_EQUAL_GREATER, //EQUAL_GREATER is for the arrow syntax

	{NULL,			NULL,			PREC_NONE}, // TOKEN_GREATER,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_GREATER_EQUAL,

	{NULL,			NULL,			PREC_NONE}, // TOKEN_LESS,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_LESS_EQUAL,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_LESS_OR, //back pipe

	//these can ONLY be doubles
	{NULL,			NULL,			PREC_NONE}, // TOKEN_AND_AND,

	{NULL,			NULL,			PREC_NONE}, // TOKEN_OR_OR,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_OR_GREATER, //pipe

	//these can single OR triple
	{NULL,			NULL,			PREC_NONE}, // TOKEN_DOT,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_DOT_DOT_DOT, //reserved for the rest/spread operator

	//ternary operator
	{NULL,			NULL,			PREC_NONE}, // TOKEN_QUESTION,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_COLON,

	//literals
	{NULL,			NULL,			PREC_NONE}, // TOKEN_IDENTIFIER,
	{number,		NULL,			PREC_NONE}, // TOKEN_NUMBER,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_STRING,

	//keywords (alphabetized)
	{NULL,			NULL,			PREC_NONE}, // TOKEN_AS,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_ASSERT,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_BREAK,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_CONST,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_CONTINUE,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_DO,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_ELSE,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_FALSE,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_FOR,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_FOREACH, //reserved
	{NULL,			NULL,			PREC_NONE}, // TOKEN_IF,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_IMPORT,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_IN, //reserved
	{NULL,			NULL,			PREC_NONE}, // TOKEN_NIL, //null
	{NULL,			NULL,			PREC_NONE}, // TOKEN_OF, //reserved
	{NULL,			NULL,			PREC_NONE}, // TOKEN_PRINT,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_RETURN,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_TRUE,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_VAR,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_WHILE,

	//meta
	{NULL,			NULL,			PREC_NONE}, // TOKEN_PASS, //do nothing
	{NULL,			NULL,			PREC_NONE}, // TOKEN_ERROR,
	{NULL,			NULL,			PREC_NONE}, // TOKEN_EOF
};

ParseRule* getRule(TokenType type) {
	return &parseRules[type];
}