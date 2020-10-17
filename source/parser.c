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
static void emitByte(Chunk* chunk, uint8_t byte, int line) {
	writeChunk(chunk, byte, line);
}

static void emitTwoBytes(Chunk* chunk, uint8_t byte1, uint8_t byte2, int line) {
	emitByte(chunk, byte1, line);
	emitByte(chunk, byte2, line);
}

static void emitLong(Chunk* chunk, uint32_t lng, int line) {
	writeChunkLong(chunk, lng, line);
}

static void emitLiteral(Chunk* chunk, Literal literal, int line) {
	//get the index of the new literal
	int index = findLiteral(&chunk->literals, literal);

	if (index < 0) {
		//new literal
		index = chunk->literals.count;
		writeLiteralArray(&chunk->literals, literal);
	}

	//free existing string literals
	freeLiteral(&literal);

	//handle > 256 literals
	if (index >= 256) {
		emitByte(chunk, (uint8_t)OP_LITERAL_LONG, line);
		emitLong(chunk, (uint32_t)index, line);
	} else {
		emitByte(chunk, (uint8_t)OP_LITERAL, line);
		emitByte(chunk, (uint8_t)index, line);
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
	int line = parser->previous.line;
	expression(parser, chunk);
	emitByte(chunk, OP_PRINT, line);
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of print statement");
}

static void expressionStmt(Parser* parser, Chunk* chunk) {
	int line = parser->previous.line;
	expression(parser, chunk); //push
	emitByte(chunk, OP_POP, line); //pop
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

static void constDecl(Parser* parser, Chunk* chunk) {
	//grab the const line
	int line = parser->previous.line;

	//constants must always be assigned a value at declaration
	consume(parser, TOKEN_IDENTIFIER, "Expected identifier name after const");
	Token identifier = parser->previous;

	if (!match(parser, TOKEN_EQUAL)) {
		error(parser, parser->current, "Expected assignment after const declaration");
		return;
	}

	//emit an expression to the chunk
	expression(parser, chunk);

	//emit the constant name as a literal
	emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(identifier.lexeme, identifier.length)), identifier.line);

	//finally, give the command to declare & assign
	emitByte(chunk, OP_CONSTANT_DECLARE, line);

	consume(parser, TOKEN_SEMICOLON, "Expected ';' at the end of a const declaration");
}

static void varDecl(Parser* parser, Chunk* chunk) {
	//grab the var line
	int line = parser->previous.line;

	consume(parser, TOKEN_IDENTIFIER, "Expected identifier name after var");
	Token identifier = parser->previous;

	//emit the variable name as a literal
	emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(identifier.lexeme, identifier.length)), identifier.line);

	//give the command to declare
	emitByte(chunk, OP_VARIABLE_DECLARE, line);

	if (match(parser, TOKEN_EQUAL)) {
		int line = parser->previous.line; //line of the equal sign

		//emit an expression to the chunk
		expression(parser, chunk);

		//give the command to assign
		emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(identifier.lexeme, identifier.length)), identifier.line);
		emitByte(chunk, OP_VARIABLE_SET, line);
	}

	consume(parser, TOKEN_SEMICOLON, "Expected ';' at the end of a var declaration");
}

static void declaration(Parser* parser, Chunk* chunk) {
	if (match(parser, TOKEN_CONST)) {
		constDecl(parser, chunk);
	} else if (match(parser, TOKEN_VAR)) {
		varDecl(parser, chunk);
	} else {
		statement(parser, chunk);
	}

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

	//if your precedence is below "assignment"
	if (canBeAssigned && match(parser, TOKEN_EQUAL)) {
		error(parser, parser->previous, "Invalid assignment target");
	}
}

//expression rules
static void string(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle interpolated strings as well
	switch(parser->previous.type) {
		case TOKEN_STRING:
			emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(parser->previous.lexeme, parser->previous.length)), parser->previous.line);
		break;

		//case TOKEN_INTERPOLATED_STRING:
			//TODO: interpolated strings
		//	break;

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
	emitLiteral(chunk, TO_NUMBER_LITERAL(strtod(buff, NULL)), parser->previous.line);
}

static void variable(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//variables can be invoked for their values, or assigned a value, or both at once
	Token identifier = parser->previous;

	//if I am being assigned
	if (match(parser, TOKEN_EQUAL)) {
		int line = parser->previous.line; //line of the equal sign

		//emit an expression to the chunk
		expression(parser, chunk);

		//give the command to assign
		emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(identifier.lexeme, identifier.length)), identifier.line);
		emitByte(chunk, OP_VARIABLE_SET, line);
	}

	//leave my value on the stack
	emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(identifier.lexeme, identifier.length)), identifier.line);
	emitByte(chunk, OP_VARIABLE_GET, identifier.line);
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
	int line = parser->previous.line;
	parsePrecendence(parser, chunk, getRule(operatorType)->precedence + 1);

	switch(operatorType) {
		//compare
		case TOKEN_EQUAL_EQUAL:
			emitByte(chunk, OP_EQUALITY, line);
			break;

		case TOKEN_BANG_EQUAL:
			emitTwoBytes(chunk, OP_EQUALITY, OP_NOT, line);
			break;

		case TOKEN_GREATER:
			emitByte(chunk, OP_GREATER, line);
			break;

		case TOKEN_GREATER_EQUAL:
			emitTwoBytes(chunk, OP_LESS, OP_NOT, line);
			break;

		case TOKEN_LESS:
			emitByte(chunk, OP_LESS, line);
			break;

		case TOKEN_LESS_EQUAL:
			emitTwoBytes(chunk, OP_GREATER, OP_NOT, line);
			break;

		//arithmetic
		case TOKEN_PLUS:
			emitByte(chunk, OP_ADD, line);
			break;

		case TOKEN_MINUS:
			emitByte(chunk, OP_SUBTRACT, line);
			break;

		case TOKEN_STAR:
			emitByte(chunk, OP_MULTIPLY, line);
			break;

		case TOKEN_SLASH:
			emitByte(chunk, OP_DIVIDE, line);
			break;

		case TOKEN_MODULO:
			emitByte(chunk, OP_MODULO, line);
			break;
	}
}

static void unary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle unary expressions
	TokenType operatorType = parser->previous.type;
	int line = parser->previous.line;

	parsePrecendence(parser, chunk, PREC_UNARY);

	switch(operatorType) {
		case TOKEN_MINUS:
			emitByte(chunk, OP_NEGATE, line);
			break;

		case TOKEN_BANG:
			emitByte(chunk, OP_NOT, line);
			break;
	}
}

static void atomic(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle atomic literals
	switch(parser->previous.type) {
		case TOKEN_NIL:
			emitLiteral(chunk, TO_NIL_LITERAL, parser->previous.line);
			break;

		case TOKEN_TRUE:
			emitLiteral(chunk, TO_BOOL_LITERAL(true), parser->previous.line);
			break;

		case TOKEN_FALSE:
			emitLiteral(chunk, TO_BOOL_LITERAL(false), parser->previous.line);
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
	{variable,	NULL,		PREC_PRIMARY},		// TOKEN_IDENTIFIER
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

	emitByte(chunk, OP_EOF, parser->previous.line); //terminate the chunk

	return chunk;
}
