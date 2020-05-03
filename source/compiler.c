#include "compiler.h"
#include "parser.h"
#include "common.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

//handle errors
void errorAt(Parser* parser, Token* token, const char* message) {
	if (parser->panicMode) return;

	//print the line
	fprintf(stderr, "[line %d] Error", token->line);

	//type
	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	} else if (token->type == TOKEN_ERROR) {
		//nothing
	} else {
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}

	//finally
	fprintf(stderr, ": %s\n", message);
	parser->hadError = true;
}

//curry the above function
void errorAtPrevious(Parser* parser, const char* message) {
	errorAt(parser, &parser->previous, message);
}

void errorAtCurrent(Parser* parser, const char* message) {
	errorAt(parser, &parser->current, message);
}

//process the stream of tokens
void advance(Parser* parser) {
	parser->previous = parser->current;

	for (;;) {
		parser->current = scanToken(parser->scanner);
		if (parser->current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser, parser->current.start);
	}
}

void consume(Parser* parser, TokenType type, const char* message) {
	if (parser->current.type == type) {
		advance(parser);
		return;
	}

	errorAtCurrent(parser, message);
}

bool match(Parser* parser, TokenType type) {
	if (!check(parser, type)) {
		return false;
	}
	advance(parser);
	return true;
}

bool check(Parser* parser, TokenType type) {
	return parser->current.type == type;
}

//exposed
bool compile(const char* source, Chunk* chunk) {
	//TODO: return bytecode for running or saving to a file
	Scanner scanner;
	Parser parser;

	//init scanner & parser
	initScanner(&scanner, source);
	initParser(&parser, &scanner, chunk);

	//process
	advance(&parser);
	while (!match(&parser, TOKEN_EOF)) {
		declaration(&parser);
	}

	//return
	emitByte(&parser, OP_RETURN);

#ifdef DEBUG_PRINT_CODE
	if (!parser.hadError) {
		disassembleChunk(chunk, "code");
	}
#endif

	bool hadError = parser.hadError;
	freeParser(&parser);

	return !hadError;
}