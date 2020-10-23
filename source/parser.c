#include "parser.h"
#include "memory.h"
#include "token_type.h"
#include "opcodes.h"

#include "literal.h"
#include "function.h"

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
static void parsePrecedence(Parser* parser, Chunk* chunk, Precedence precedence);

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

static int emitLiteral(Chunk* chunk, Literal literal, int line) {
	//get the index of the new literal
	int index = findLiteral(&chunk->literals, literal);

	if (index < 0) {
		//new literal
		index = chunk->literals.count;
		writeLiteralArray(&chunk->literals, literal);
	}

	//free existing string literals
	freeLiteral(literal);

	//handle > 256 literals
	if (index >= 256) {
		emitByte(chunk, (uint8_t)OP_LITERAL_LONG, line);
		emitLong(chunk, (uint32_t)index, line);
	} else {
		emitByte(chunk, (uint8_t)OP_LITERAL, line);
		emitByte(chunk, (uint8_t)index, line);
	}

	return index;
}

static void overwriteLong(Chunk* chunk, int index, uint32_t value, int line) {
	overwriteChunkLong(chunk, index, value, line);
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

//forward declare as a kind of entry point
static void declaration(Parser* parser, Chunk* chunk, int popScopesOnReturn);
static void expression(Parser* parser, Chunk* chunk);

static Function* readFunctionCode(Parser* parser, Function* func) {
	//read the code into the function's chunk
	if (match(parser, TOKEN_LEFT_BRACE)) {
		//for error handling
		Token opening = parser->previous;

		//process the grammar rules
		while (!match(parser, TOKEN_RIGHT_BRACE)) {
			if (match(parser, TOKEN_EOF)) {
				error(parser, opening, "Expected closing '}' to match opening '{' in function declaration");
				return NULL;
			}

			declaration(parser, func->chunk, 0);
		}

		//if the last statement of the function was not a return, insert a null return at the end
		if (func->chunk->count == 0 || func->chunk->code[func->chunk->count - 1] != OP_RETURN) {
			emitLiteral(func->chunk, TO_NIL_LITERAL, parser->previous.line); //leave a null
			emitByte(func->chunk, OP_RETURN, parser->previous.line); //terminate the chunk
		}
	} else {
		//single-line expression
		expression(parser, func->chunk);
		emitByte(func->chunk, OP_RETURN, parser->previous.line);
	}

	return func;
}

//grammar statement rules
static void breakStmt(Parser* parser, Chunk* chunk) {
	//TODO: implement this
}

static void continueStmt(Parser* parser, Chunk* chunk) {
	//TODO: implement this
}

static void doStmt(Parser* parser, Chunk* chunk) {
	//TODO: implement this
}

static void forStmt(Parser* parser, Chunk* chunk) {
	//TODO: implement this
}

static void ifStmt(Parser* parser, Chunk* chunk) {
	//push the expression
	consume(parser, TOKEN_LEFT_PAREN, "Expected '(' after if statement");
	expression(parser, chunk);
	consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after if expression");

	//catch the opcode parameter
	emitByte(chunk, OP_IF_FALSE_JUMP, parser->previous.line);
	int index = chunk->count;
	emitLong(chunk, 0, parser->previous.line); //dummy parameter

	//body
	emitByte(chunk, OP_SCOPE_BEGIN, parser->previous.line);
	declaration(parser, chunk, true);
	emitByte(chunk, OP_SCOPE_END, parser->previous.line);

	overwriteLong(chunk, index, (uint32_t)chunk->count, parser->previous.line);

	//handle else
	if (match(parser, TOKEN_ELSE)) { //same as above
		//catch the opcode parameter
		emitByte(chunk, OP_IF_FALSE_JUMP, parser->previous.line);
		int index = chunk->count;
		emitLong(chunk, 0, parser->previous.line); //dummy parameter

		//body
		emitByte(chunk, OP_SCOPE_BEGIN, parser->previous.line);
		declaration(parser, chunk, true);
		emitByte(chunk, OP_SCOPE_END, parser->previous.line);

		overwriteLong(chunk, index, (uint32_t)chunk->count, parser->previous.line);
	}
}

static void printStmt(Parser* parser, Chunk* chunk) {
	int line = parser->previous.line;
	expression(parser, chunk);
	emitByte(chunk, OP_PRINT, line);
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of print statement");
}

static void returnStmt(Parser* parser, Chunk* chunk, int popScopesOnReturn) {
	//leave an exrpession or a null on the stack
	int line = parser->previous.line;

	while(popScopesOnReturn > 0) {
		emitByte(chunk, OP_SCOPE_END, parser->previous.line);
		popScopesOnReturn -= 1;
	}

	if (!match(parser, TOKEN_SEMICOLON)) {
		expression(parser, chunk);
		emitByte(chunk, OP_RETURN, line);
		consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of return statement");
	} else {
		emitLiteral(chunk, TO_NIL_LITERAL, line); //leave a null
		emitByte(chunk, OP_RETURN, line);
	}
}

static void whileStmt(Parser* parser, Chunk* chunk) {
	//TODO: implement this
}

static void block(Parser* parser, Chunk* chunk, int popScopesOnReturn) {
	Token opening = parser->previous;

	emitByte(chunk, OP_SCOPE_BEGIN, opening.line);

	while(!match(parser, TOKEN_RIGHT_BRACE)) {
		if (match(parser, TOKEN_EOF)) {
			error(parser, opening, "Expected closing '}' to match opening '{'");
			return;
		}

		declaration(parser, chunk, popScopesOnReturn + 1);
	}

	emitByte(chunk, OP_SCOPE_END, parser->previous.line);
}

static void expressionStmt(Parser* parser, Chunk* chunk) {
	int line = parser->previous.line;
	expression(parser, chunk); //push
	emitByte(chunk, OP_POP, line); //pop
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at the end of an expression statement");
}

//high-level grammar rules
static void statement(Parser* parser, Chunk* chunk, int popScopesOnReturn) {
	if (match(parser, TOKEN_BREAK)) {
		breakStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_CONTINUE)) {
		continueStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_DO)) {
		doStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_FOR)) {
		forStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_IF)) {
		ifStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_PRINT)) {
		printStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_RETURN)) {
		returnStmt(parser, chunk, popScopesOnReturn);
		return;
	}

	if (match(parser, TOKEN_WHILE)) {
		whileStmt(parser, chunk);
		return;
	}

	if (match(parser, TOKEN_LEFT_BRACE)) {
		block(parser, chunk, popScopesOnReturn);
		return;
	}

	//simple empty statements
	if (match(parser, TOKEN_SEMICOLON)) {
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

static void declaration(Parser* parser, Chunk* chunk, int popScopesOnReturn) {
	if (match(parser, TOKEN_CONST)) {
		constDecl(parser, chunk);
	} else if (match(parser, TOKEN_VAR)) {
		varDecl(parser, chunk);
	} else {
		statement(parser, chunk, popScopesOnReturn);
	}

	if (parser->panic) {
		synchronize(parser);
	}
}

//refer to the grammar expression rules below
static void expression(Parser* parser, Chunk* chunk) {
	//delegate to the pratt table for expression precedence
	parsePrecedence(parser, chunk, PREC_ASSIGNMENT);
}

//precedence
static void parsePrecedence(Parser* parser, Chunk* chunk, Precedence precedence) {
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
		ParseFn infixRule = getRule(parser->current.type)->infix;

		if (infixRule == NULL) {
			error(parser, parser->current, "Expected operator");
			return;
		}

		infixRule(parser, chunk, canBeAssigned); //NOTE: infix rule must advance the parser
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

	//variables can also lead off function declarations
	if (match(parser, TOKEN_EQUAL_GREATER)) {
		Token op = parser->previous;

		Function* func = ALLOCATE(Function, 1);
		initFunction(func);

		//read the parameter
		char* buffer = ALLOCATE(char, identifier.length + 1);
		sprintf(buffer, "%.*s", identifier.length, identifier.lexeme);
		writeLiteralArray(&func->parameters, TO_STRING_LITERAL(buffer));
		FREE(char, buffer);

		//read the code
		func = readFunctionCode(parser, func);

		if (func != NULL) {
			emitLiteral(chunk, TO_FUNCTION_PTR(func), op.line);
		}

		emitByte(chunk, OP_FUNCTION_DECLARE, op.line);
		return;
	}

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

static void groupingPrefix(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//this can be a function?
	Function* func = ALLOCATE(Function, 1);
	initFunction(func);
	bool canBeFunction = true;

	//for errors
	Token leftParen = parser->previous;

	//begin the grouping
	emitByte(chunk, OP_GROUPING_BEGIN, parser->previous.line);

	//check for functions with 0+ parameters signalled by ()
	if (!match(parser, TOKEN_RIGHT_PAREN)) {
		//handle groupings - expressions or function parameters
		while(!match(parser, TOKEN_EOF)) {

			//TODO: fibonacci problem

			//get the identifier OR expression
			if (match(parser, TOKEN_IDENTIFIER)) {
				//store all identifiers as strings, because why not?
				char* buffer = ALLOCATE(char, parser->previous.length + 1);
				sprintf(buffer, "%.*s", parser->previous.length, parser->previous.lexeme);

				//check for duplicate parameter names
				for (int i = 0; i < func->parameters.count; i++) {
					if (strcmp(AS_STRING(func->parameters.literals[i]), buffer) == 0) {
						error(parser, parser->previous, "Can't have duplicate parameter names in a function declaration");
						break;
					}
				}

				if (parser->panic) { //double loops
					break;
				}

				//write to the nascent chunk and functions, both
				writeLiteralArray(&func->parameters, TO_STRING_LITERAL(buffer)); //parameter names
				emitLiteral(chunk, TO_STRING_LITERAL(buffer), parser->previous.line);
				emitByte(chunk, OP_PARAMETER_DECLARE, parser->previous.line); //get the actual value on stack OR pop it as a parameter
			} else {
				canBeFunction = false;
				expression(parser, chunk);
			}

			if (match(parser, TOKEN_RIGHT_PAREN)) {
				break;
			}

			if (!match(parser, TOKEN_COMMA)) {
				error(parser, parser->current, "Expected ',' in grouping");
				break;
			}
		}
	}

	//end the grouping
	emitByte(chunk, OP_GROUPING_END, parser->previous.line);

	//this might be a function
	if (match(parser, TOKEN_EQUAL_GREATER)) {
		if (canBeFunction) {
			Token op = parser->previous;

			func = readFunctionCode(parser, func);

			if (func != NULL) {
				emitLiteral(chunk, TO_FUNCTION_PTR(func), op.line);
				emitByte(chunk, OP_FUNCTION_DECLARE, op.line); //ah crap.
			}
		} else {
			freeFunction(func);
			error(parser, leftParen, "Incorrect parameters for function declaration");
		}
	} else {
		freeFunction(func);
	}
}

static void groupingInfix(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	advance(parser);
	groupingPrefix(parser, chunk, canBeAssigned);
}

static void binary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//grab the previous token for the compound assignments
	Token previous = parser->previous;

	//handle binary expressions
	TokenType operatorType = parser->current.type;
	int line = parser->current.line;

	//handle the right-side of the operator
	advance(parser);
	parsePrecedence(parser, chunk, getRule(operatorType)->precedence + 1);

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

		//complex stuff repeated, so put it in a temp macro
#define COMPOUND_ASSIGNMENT(COMPOUND_OPERATOR) \
			if (previous.type != TOKEN_IDENTIFIER) { \
				error(parser, previous, "Expected identifier on left-hand side of compound assignment operator"); \
				break; \
			} \
			emitByte(chunk, COMPOUND_OPERATOR, line); \
			emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(previous.lexeme, previous.length)), previous.line); \
			emitByte(chunk, OP_VARIABLE_SET, line); \
			emitLiteral(chunk, TO_STRING_LITERAL(copyAndParseString(previous.lexeme, previous.length)), previous.line); \
			emitByte(chunk, OP_VARIABLE_GET, line);

		case TOKEN_PLUS_EQUAL:
			COMPOUND_ASSIGNMENT(OP_ADD);
			break;

		case TOKEN_MINUS_EQUAL:
			COMPOUND_ASSIGNMENT(OP_SUBTRACT);
			break;

		case TOKEN_STAR_EQUAL:
			COMPOUND_ASSIGNMENT(OP_MULTIPLY);
			break;

		case TOKEN_SLASH_EQUAL:
			COMPOUND_ASSIGNMENT(OP_DIVIDE);
			break;

		case TOKEN_MODULO_EQUAL:
			COMPOUND_ASSIGNMENT(OP_MODULO);
			break;

#undef COMPOUND_ASSIGNMENT
	}
}

static void unary(Parser* parser, Chunk* chunk, bool canBeAssigned) {
	//handle unary expressions
	TokenType operatorType = parser->previous.type;
	int line = parser->previous.line;

	parsePrecedence(parser, chunk, PREC_UNARY);

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
	{groupingPrefix,	groupingInfix,	PREC_CALL},			// TOKEN_LEFT_PAREN
	{NULL,				NULL,			PREC_NONE},			// TOKEN_RIGHT_PAREN
	{NULL,				NULL,			PREC_NONE},			// TOKEN_LEFT_BRACE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_RIGHT_BRACE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_LEFT_BRACKET
	{NULL,				NULL,			PREC_NONE},			// TOKEN_RIGHT_BRACKET
	{NULL,				NULL,			PREC_NONE},			// TOKEN_SEMICOLON
	{NULL,				NULL,			PREC_NONE},			// TOKEN_COMMA
	{NULL,				binary,			PREC_TERM},			// TOKEN_PLUS
	{NULL,				binary,			PREC_ASSIGNMENT},	// TOKEN_PLUS_EQUAL
	{NULL,				NULL,			PREC_NONE},			// TOKEN_PLUS_PLUS
	{unary,				binary,			PREC_TERM},			// TOKEN_MINUS
	{NULL,				binary,			PREC_ASSIGNMENT},	// TOKEN_MINUS_EQUAL
	{NULL,				NULL,			PREC_NONE},			// TOKEN_MINUS_MINUS
	{NULL,				binary,			PREC_FACTOR},		// TOKEN_STAR
	{NULL,				binary,			PREC_ASSIGNMENT},	// TOKEN_STAR_EQUAL
	{NULL,				binary,			PREC_FACTOR},		// TOKEN_SLASH
	{NULL,				binary,			PREC_ASSIGNMENT},	// TOKEN_SLASH_EQUAL
	{NULL,				binary,			PREC_FACTOR},		// TOKEN_MODULO
	{NULL,				binary,			PREC_ASSIGNMENT},	// TOKEN_MODULO_EQUAL
	{unary,				NULL,			PREC_NONE},			// TOKEN_BANG
	{NULL,				binary,			PREC_EQUALITY},		// TOKEN_BANG_EQUAL
	{NULL,				NULL,			PREC_NONE},			// TOKEN_EQUAL
	{NULL,				binary,			PREC_EQUALITY},		// TOKEN_EQUAL_EQUAL
	{NULL,				NULL,			PREC_NONE},			// TOKEN_EQUAL_GREATER
	{NULL,				binary,			PREC_COMPARISON},	// TOKEN_GREATER
	{NULL,				binary,			PREC_COMPARISON},	// TOKEN_GREATER_EQUAL
	{NULL,				binary,			PREC_COMPARISON},	// TOKEN_LESS
	{NULL,				binary,			PREC_COMPARISON},	// TOKEN_LESS_EQUAL
	{NULL,				NULL,			PREC_NONE},			// TOKEN_LESS_OR
	{NULL,				NULL,			PREC_NONE},			// TOKEN_AND_AND
	{NULL,				NULL,			PREC_NONE},			// TOKEN_OR_OR
	{NULL,				NULL,			PREC_NONE},			// TOKEN_OR_GREATER
	{NULL,				NULL,			PREC_NONE},			// TOKEN_DOT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_DOT_DOT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_DOT_DOT_DOT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_QUESTION
	{NULL,				NULL,			PREC_NONE},			// TOKEN_COLON
	{NULL,				NULL,			PREC_NONE},			// TOKEN_COLON_COLON
	{variable,			NULL,			PREC_PRIMARY},		// TOKEN_IDENTIFIER
	{number,			NULL,			PREC_PRIMARY},		// TOKEN_NUMBER
	{string,			NULL,			PREC_PRIMARY},		// TOKEN_STRING
	{string,			NULL,			PREC_PRIMARY},		// TOKEN_INTERPOLATED_STRING
	{NULL,				NULL,			PREC_NONE},			// TOKEN_AS
	{NULL,				NULL,			PREC_NONE},			// TOKEN_ASSERT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_ASYNC
	{NULL,				NULL,			PREC_NONE},			// TOKEN_AWAIT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_BREAK
	{NULL,				NULL,			PREC_NONE},			// TOKEN_CASE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_CONST
	{NULL,				NULL,			PREC_NONE},			// TOKEN_CONTINUE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_DEFAULT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_DO
	{NULL,				NULL,			PREC_NONE},			// TOKEN_ELSE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_EXPORT
	{atomic,			NULL,			PREC_PRIMARY},		// TOKEN_FALSE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_FOR
	{NULL,				NULL,			PREC_NONE},			// TOKEN_FOREACH
	{NULL,				NULL,			PREC_NONE},			// TOKEN_IF
	{NULL,				NULL,			PREC_NONE},			// TOKEN_IMPORT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_IN
	{atomic,			NULL,			PREC_PRIMARY},		// TOKEN_NIL
	{NULL,				NULL,			PREC_NONE},			// TOKEN_OF
	{NULL,				NULL,			PREC_NONE},			// TOKEN_PRINT
	{NULL,				NULL,			PREC_NONE},			// TOKEN_PURE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_RETURN
	{NULL,				NULL,			PREC_NONE},			// TOKEN_SWITCH
	{atomic,			NULL,			PREC_PRIMARY},		// TOKEN_TRUE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_VAR
	{NULL,				NULL,			PREC_NONE},			// TOKEN_WHILE
	{NULL,				NULL,			PREC_NONE},			// TOKEN_PASS
	{NULL,				NULL,			PREC_NONE},			// TOKEN_ERROR
	{NULL,				NULL,			PREC_NONE},			// TOKEN_EOF
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
		declaration(parser, chunk, 0);
	}

	emitByte(chunk, OP_EOF, parser->previous.line); //terminate the chunk

	return chunk;
}
