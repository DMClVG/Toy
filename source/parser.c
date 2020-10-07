#include "parser.h"
#include "memory.h"
#include "token_type.h"
#include "opcodes.h"

#include <stdio.h>

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

//grammar expression rules
static void expression(Parser* parser, Chunk* chunk) {
	//TODO: delegate to the pratt table for expression precedence
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

	advance(parser); //prime the first input

	//process the grammar rules
	while (!match(parser, TOKEN_EOF)) {
		declaration(parser, chunk);
	}

	emitByte(parser, chunk, OP_RETURN); //terminate the chunk (placeholder)

	return chunk;
}

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