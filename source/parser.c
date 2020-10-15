#include "parser.h"
#include "memory.h"
#include "token_type.h"
#include "opcodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* DOCS: The original build of this had the parser spread between a dozen files - I'm trying to prevent that.
*/

//string utilities
char* copyAndParseString(char* original, int originalLength) {
	//get the length of the new buffer
	int newLength = 0;
	for (int i = 0; original[i] && i < originalLength; i++) {
		if (original[i] == '\\') {
			i++;
		}
		newLength++;
	}

	//print each char into a new buffer
	char* buffer = ALLOCATE(char, newLength + 1);
	char* ptr = buffer;

	for (int i = 0; original[i] && i < originalLength; i++) {
		//escaped char
		if (original[i] == '\\') {
			i++;
			switch(original[i]) {
				case 'a': *ptr = '\a'; break;
				case 'b': *ptr = '\b'; break;
				case 'f': *ptr = '\f'; break;
				case 'n': *ptr = '\n'; break;
				case 'r': *ptr = '\r'; break;
				case 't': *ptr = '\t'; break;
				case 'v': *ptr = '\v'; break;
				case '\\': *ptr = '\\'; break;
				case '\'': *ptr = '\''; break;
				case '"': *ptr = '"'; break;
				case '`': *ptr = '`'; break;
			}
		} else {
			*ptr = original[i];
		}

		ptr++;
	}

	//terminate the string
	*ptr = '\0';

	return buffer;
}

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

static void emitTwoBytes(Parser* parser, Chunk* chunk, uint8_t byte1, uint8_t byte2) {
	emitByte(parser, chunk, byte1);
	emitByte(parser, chunk, byte2);
}

static void emitLong(Parser* parser, Chunk* chunk, uint32_t lng) {
	writeChunkLong(chunk, lng, parser->lexer->line);
}

static void emitLiteral(Parser* parser, Chunk* chunk, Literal literal) {
	//get the index of the new literal
	int index = findLiteral(&chunk->literals, literal);

	if (index < 0) {
		//new literal
		index = chunk->literals.count;
		writeLiteralArray(&chunk->literals, literal);
	} else {
		//free existing string literals
		//TODO: interpolated strings
		if (literal.type == LITERAL_STRING) {
			char* str = AS_STRING(literal);
			FREE_ARRAY(char, str, strlen(str));
		}
	}

	//handle > 256 literals
	if (index >= 256) {
		emitByte(parser, chunk, (uint8_t)OP_LITERAL_LONG);
		emitLong(parser, chunk, (uint32_t)index);
	} else {
		emitByte(parser, chunk, (uint8_t)OP_LITERAL);
		emitByte(parser, chunk, (uint8_t)index);
	}
}

//parsing utilities
static void error(Parser* parser, Token token, const char* message) {
	//keep going while panicing
	if (parser->panic) return;

	fprintf(stderr, "[Line %d] Error", token.line);

	//check type
	if (token.type == TOKEN_EOF) {
		fprintf(stderr, " at end");
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
	emitByte(parser, chunk, OP_PRINT);
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of print statement");
}

static void expressionStmt(Parser* parser, Chunk* chunk) {
	expression(parser, chunk); //push
	emitByte(parser, chunk, OP_POP); //pop
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at the end of an expression statement");
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
static void string(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle interpolated strings as well
	switch(parser->previous.type) {
		case TOKEN_STRING:
			emitLiteral(parser, chunk, TO_STRING_LITERAL(copyAndParseString(parser->previous.lexeme, parser->previous.length)));
			break;

//		case TOKEN_INTERPOLATED_STRING:
//			//TODO: interpolated strings
//			break;

		default:
			error(parser, parser->previous, "Unexpected token passed to string");
	}
}

static void number(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle numbers

	//copy the lexeme to a tmp buffer
	char buff[32]; //enough space?
	sprintf(buff, "%.*s\0", parser->previous.length, parser->previous.lexeme);

	//output the number & emit it
	double number = strtod(buff, NULL);
	emitLiteral(parser, chunk, TO_NUMBER_LITERAL(number));
}

static void variable(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//TODO: variables
}

static void grouping(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle groupings
	expression(parser, chunk);
	consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after grouping");
}

static void binary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle binary expressions

	//handle the left-side of the operator
	TokenType operatorType = parser->previous.type;
	parsePrecendence(parser, chunk, getRule(operatorType)->precedence + 1);

	switch(operatorType) {
		//compare
		case TOKEN_EQUAL_EQUAL:
			emitByte(parser, chunk, OP_EQUALITY);
			break;

		case TOKEN_BANG_EQUAL:
			emitTwoBytes(parser, chunk, OP_EQUALITY, OP_NOT);
			break;

		case TOKEN_GREATER:
			emitByte(parser, chunk, OP_GREATER);
			break;

		case TOKEN_GREATER_EQUAL:
			emitTwoBytes(parser, chunk, OP_LESS, OP_NOT);
			break;

		case TOKEN_LESS:
			emitByte(parser, chunk, OP_LESS);
			break;

		case TOKEN_LESS_EQUAL:
			emitTwoBytes(parser, chunk, OP_GREATER, OP_NOT);
			break;

		//arithmetic
		case TOKEN_PLUS:
			emitByte(parser, chunk, OP_ADD);
			break;

		case TOKEN_MINUS:
			emitByte(parser, chunk, OP_SUBTRACT);
			break;

		case TOKEN_STAR:
			emitByte(parser, chunk, OP_MULTIPLY);
			break;

		case TOKEN_SLASH:
			emitByte(parser, chunk, OP_DIVIDE);
			break;

		case TOKEN_MODULO:
			emitByte(parser, chunk, OP_MODULO);
			break;
	}
}

static void unary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle unary expressions
	TokenType operatorType = parser->previous.type;

	parsePrecendence(parser, chunk, PREC_UNARY);

	switch(operatorType) {
		case TOKEN_MINUS:
			emitByte(parser, chunk, OP_NEGATE);
			break;

		case TOKEN_BANG:
			emitByte(parser, chunk, OP_NOT);
			break;
	}
}

static void atomic(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle atomic literals
	switch(parser->previous.type) {
		case TOKEN_NIL:
			emitLiteral(parser, chunk, TO_NIL_LITERAL);
			break;

		case TOKEN_TRUE:
			emitLiteral(parser, chunk, TO_BOOL_LITERAL(true));
			break;

		case TOKEN_FALSE:
			emitLiteral(parser, chunk, TO_BOOL_LITERAL(false));
			break;
	}
}

//a pratt table
ParseRule parseRules[] = {
	{grouping,	NULL,		PREC_CALL},			// TOKEN_LEFT_PAREN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RIGHT_PAREN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LEFT_BRACE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RIGHT_BRACE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LEFT_BRACKET
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RIGHT_BRACKET
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SEMICOLON
	{NULL,		NULL,		PREC_NONE},			// TOKEN_COMMA
	{NULL,		binary,		PREC_TERM},			// TOKEN_PLUS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PLUS_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PLUS_PLUS
	{unary,		binary,		PREC_TERM},			// TOKEN_MINUS
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MINUS_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MINUS_MINUS
	{NULL,		binary,		PREC_FACTOR},		// TOKEN_STAR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_STAR_EQUAL
	{NULL,		binary,		PREC_FACTOR},		// TOKEN_SLASH
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SLASH_EQUAL
	{NULL,		binary,		PREC_FACTOR},		// TOKEN_MODULO
	{NULL,		NULL,		PREC_NONE},			// TOKEN_MODULO_EQUAL
	{unary,		NULL,		PREC_NONE},			// TOKEN_BANG
	{NULL,		binary,		PREC_EQUALITY},		// TOKEN_BANG_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EQUAL
	{NULL,		binary,		PREC_EQUALITY},		// TOKEN_EQUAL_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_EQUAL_GREATER
	{NULL,		binary,		PREC_COMPARISON},	// TOKEN_GREATER
	{NULL,		binary,		PREC_COMPARISON},	// TOKEN_GREATER_EQUAL
	{NULL,		binary,		PREC_COMPARISON},	// TOKEN_LESS
	{NULL,		binary,		PREC_COMPARISON},	// TOKEN_LESS_EQUAL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_LESS_OR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_AND_AND
	{NULL,		NULL,		PREC_NONE},			// TOKEN_OR_OR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_OR_GREATER
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DOT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DOT_DOT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_DOT_DOT_DOT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_QUESTION
	{NULL,		NULL,		PREC_NONE},			// TOKEN_COLON
	{NULL,		NULL,		PREC_NONE},			// TOKEN_COLON_COLON
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IDENTIFIER
	{number,	NULL,		PREC_PRIMARY},		// TOKEN_NUMBER
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
	{atomic,	NULL,		PREC_PRIMARY},		// TOKEN_FALSE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_FOR
	{NULL,		NULL,		PREC_NONE},			// TOKEN_FOREACH
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IF
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IMPORT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_IN
	{atomic,	NULL,		PREC_PRIMARY},		// TOKEN_NIL
	{NULL,		NULL,		PREC_NONE},			// TOKEN_OF
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PRINT
	{NULL,		NULL,		PREC_NONE},			// TOKEN_PURE
	{NULL,		NULL,		PREC_NONE},			// TOKEN_RETURN
	{NULL,		NULL,		PREC_NONE},			// TOKEN_SWITCH
	{atomic,	NULL,		PREC_PRIMARY},		// TOKEN_TRUE
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

	emitByte(parser, chunk, OP_EOF); //terminate the chunk

	return chunk;
}
