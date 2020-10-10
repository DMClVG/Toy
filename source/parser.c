#include "parser.h"
#include "memory.h"
#include "token_type.h"
#include "opcodes.h"

#include <stdio.h>

/* DOCS: The original build of this had the parser spread between a dozen files - I'm trying to prevent that.
*/

//forward declare the precedent rules
typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_TERNARY,
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
typedef void (*ParseFn)(Parser* parser, Chunk* chunk, bool canBeAssigned);

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

//get the precedence rule for each token type
static ParseRule* getRule(TokenType type);
static void parsePrecendence(Parser* parser, Chunk* chunk, Precedence precedence);

//convenience
static void emitByte(Parser* parser, Chunk* chunk, uint8_t byte) {
	writeChunk(chunk, byte, parser->lexer->line);
}

static void emitTwoByte(Parser* parser, Chunk* chunk, uint8_t byte1, uint8_t byte2) {
	emitByte(parser, chunk, byte1);
	emitByte(parser, chunk, byte2);
}

static void emitLong(Parser* parser, Chunk* chunk, uint32_t lng) {
	writeChunkLong(chunk, lng, parser->lexer->line);
}

//utilities
static void error(Parser* parser, Token token, const char* message) {
	//keep going while panicing
	if (parser->panic) return;

	fprintf(stderr, "[line %d] Error", token.line);

	//check type
	if (token.type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	}

	else if (token.type == TOKEN_ERROR) {
		//print nothing?
	}

	else {
		fprintf(stderr, " at '%.*s'", token.length, token.lexeme);
	}

	//finally
	fprintf(stderr, ": %s\n", message);
	parser->error = true;
	parser->panic = true;
}

static void advance(Parser* parser) {
	parser->previous = parser->current;
	parser->current = scanLexer(parser->lexer);

	if (parser->current.type == TOKEN_ERROR) {
		error(parser, parser->current, "Lexer error");
	}
}

static bool match(Parser* parser, TokenType tokenType) {
	if (parser->current.type == tokenType) {
		advance(parser);
		return true;
	}
	return false;
}

static void consume(Parser* parser, TokenType tokenType, const char* msg) {
	if (parser->current.type != tokenType) {
		error(parser, parser->current, msg);
		return;
	}

	advance(parser);
}

static void synchronize(Parser* parser) {
	while (parser->current.type != TOKEN_EOF) {
		switch(parser->current.type) {
			//these tokens can start a line
			case TOKEN_ASSERT:
			case TOKEN_BREAK:
			case TOKEN_CONST:
			case TOKEN_CONTINUE:
			case TOKEN_DO:
			case TOKEN_FOR:
			case TOKEN_FOREACH:
			case TOKEN_IF:
			case TOKEN_IMPORT:
			case TOKEN_PRINT:
			case TOKEN_RETURN:
			case TOKEN_SWITCH:
			case TOKEN_VAR:
			case TOKEN_WHILE:
				parser->panic = false;
				return;

			default:
				advance(parser);
		}
	}
}

//refer to the grammar expression rules below
static void expression(Parser* parser, Chunk* chunk) {
	//delegate to the pratt table for expression precedence
	parsePrecendence(parser, chunk, PREC_ASSIGNMENT);
}

//grammar statement rules
static void printStmt(Parser* parser, Chunk* chunk) {
	expression(parser, chunk);
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of print statement");
	emitByte(parser, chunk, OP_PRINT);
}

static void expressionStmt(Parser* parser, Chunk* chunk) {
	expression(parser, chunk); //push
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at the end of an expression statement");
	emitByte(parser, chunk, OP_POP); //pop
}

//high-level grammar rules
static void statement(Parser* parser, Chunk* chunk) {
	if (match(parser, TOKEN_PRINT)) {
		printStmt(parser, chunk);
		return;
	}

	//default
	expressionStmt(parser, chunk);
}

static void declaration(Parser* parser, Chunk* chunk) {
	//TODO: other grammar rules
	statement(parser, chunk);

	if (parser->panic) {
		synchronize(parser);
	}
}

//precedence
static void parsePrecendence(Parser* parser, Chunk* chunk, Precedence precedence) {
	//every expression has a prefix rule
	advance(parser);
	ParseFn prefixRule = getRule(parser->previous.type)->prefix;
	if (prefixRule == NULL) {
		error(parser, parser->previous, "Expected expression");
		return;
	}

	bool canBeAssigned = precedence <= PREC_ASSIGNMENT;
	prefixRule(parser, chunk, canBeAssigned);

	//infix rules are left-recursive
	while (precedence <= getRule(parser->current.type)->precedence) {
		advance(parser);
		ParseFn infixRule = getRule(parser->previous.type)->infix;
		infixRule(parser, chunk, canBeAssigned);
	}

	//if your precedence is above "assignment"
	if (canBeAssigned && match(parser, TOKEN_EQUAL)) {
		error(parser, parser->previous, "Invalid assignment target");
	}
}

//expression rules
static void number(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: numbers
}

static void string(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: strings
	printf("MARK 1\n");
}

static void variable(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: variables
}

static void grouping(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: groupings
}

static void unary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: unary
}

static void binary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: binary
}

static void atom(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: atom
}

//a pratt table
ParseRule parseRules[] = {
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LEFT_PAREN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RIGHT_PAREN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LEFT_BRACE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RIGHT_BRACE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LEFT_BRACKET
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RIGHT_BRACKET
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SEMICOLON
	{NULL,		NULL,		PREC_NONE},			// TOKEN_COMMA
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PLUS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PLUS_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PLUS_PLUS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MINUS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MINUS_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MINUS_MINUS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_STAR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_STAR_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SLASH
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SLASH_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MODULO
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MODULO_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_BANG
	{NULL,		NULL,		PREC_NONE},			// TOKEN_BANG_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EQUAL_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EQUAL_GREATER
	{NULL,		NULL,		PREC_NONE},			// TOKEN_GREATER
	{NULL,		NULL,		PREC_NONE},			// TOKEN_GREATER_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LESS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LESS_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LESS_OR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_AND_AND
	{NULL,		NULL,		PREC_NONE},			// TOKEN_OR_OR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_OR_GREATER
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DOT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DOT_DOT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DOT_DOT_DOT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_QUESTION
	{NULL,		NULL,		PREC_NONE},			// TOKEN_COLON
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IDENTIFIER
	{NULL,		NULL,		PREC_NONE},			// TOKEN_NUMBER
	{string,	NULL,		PREC_PRIMARY},		// TOKEN_STRING
	{string,	NULL,		PREC_PRIMARY},		// TOKEN_INTERPOLATED_STRING
	{NULL,		NULL,		PREC_NONE},			// TOKEN_AS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_ASSERT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_ASYNC
	{NULL,		NULL,		PREC_NONE},			// TOKEN_AWAIT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_BREAK
	{NULL,		NULL,		PREC_NONE},			// TOKEN_CASE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_CONST
	{NULL,		NULL,		PREC_NONE},			// TOKEN_CONTINUE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DEFAULT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DO
	{NULL,		NULL,		PREC_NONE},			// TOKEN_ELSE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EXPORT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_FALSE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_FOR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_FOREACH
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IF
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IMPORT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_NIL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_OF
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PRINT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PURE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RETURN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SWITCH
	{NULL,		NULL,		PREC_NONE},			// TOKEN_TRUE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_VAR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_WHILE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PASS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_ERROR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EOF
};

ParseRule* getRule(TokenType type) {
	return &parseRules[type];
}

//exposed functions
void initParser(Parser* parser, Lexer* lexer) {
	parser->lexer = lexer;
	parser->error = false;
	parser->panic = false;
}

void freeParser(Parser* parser) {
	initParser(parser, NULL);
}

Chunk* scanParser(Parser* parser) {
	Chunk* chunk = ALLOCATE(Chunk, 1);
	initChunk(chunk);

	advance(parser); //prime the first input

	//process the grammar rules
	while (!match(parser, TOKEN_EOF)) {
		declaration(parser, chunk);
	}

	emitByte(parser, chunk, OP_RETURN); //terminate the chunk (placeholder)

	return chunk;
}

//TODO: move chunks into their own file
void initChunk(Chunk* chunk) {
	chunk->capacity = 0;
	chunk->count = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	initLiteralArray(&chunk->literals);
	initTable(&chunk->variables);
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeLiteralArray(&chunk->literals);
	freeTable(&chunk->variables);
	initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t val, int line) {
	//grow the arrays if necessary
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	//write a byte
	chunk->code[chunk->count] = val;
	chunk->lines[chunk->count] = line;
	chunk->count++;
}

void writeChunkLong(Chunk* chunk, uint32_t val, int line) {
	//grow the arrays if necessary
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;

		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}

	//write a uint32
	*(uint32_t*)(chunk->code + chunk->count) = val;
	chunk->lines[chunk->count] = line;
	chunk->count += sizeof(uint32_t);
}
