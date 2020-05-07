#include "parser_tools.h"
#include "parser.h"
#include "common.h"
#include "memory.h"
#include "grammar_rules.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//error recovery
void synchronize(Parser* parser) {
	parser->panicMode = false;

	while(parser->current.type != TOKEN_EOF) {
		if (parser->previous.type == TOKEN_SEMICOLON) return;

		switch(parser->current.type) {
			//NOTE: statements begin with these
			case TOKEN_ASSERT:
			case TOKEN_BREAK:
			case TOKEN_CONST:
			case TOKEN_CONTINUE:
			case TOKEN_DO:
			case TOKEN_FOR:
			case TOKEN_FOREACH:
			case TOKEN_IF:
			case TOKEN_IMPORT:
			case TOKEN_RETURN:
			case TOKEN_VAR:
			case TOKEN_WHILE:
			case TOKEN_PASS:
				return;

			default:
				//do nothing
				;
		}

		advance(parser);
	}
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

//convenience
void emitByte(Parser* parser, uint8_t byte) {
	writeChunk(parser->chunk, byte, parser->previous.line);
}

void emitBytes(Parser* parser, uint8_t byte1, uint8_t byte2) {
	emitByte(parser, byte1);
	emitByte(parser, byte2);
}

void emitLong(Parser* parser, uint32_t lng) {
	writeChunkLong(parser->chunk, lng, parser->previous.line);
}

uint32_t emitConstant(Parser* parser, Value value) {
	return writeConstant(parser->chunk, value, parser->previous.line);
}

uint32_t idenifierConstant(Parser* parser, Token* name) { //possibly search up existing constant
	return emitConstant(parser, OBJECT_VAL(copyString(&parser->chunk->objects, &parser->chunk->strings, name->start, name->length)));
}

bool identifiersEqual(Token* a, Token* b) {
	if (a->length != b->length) return false;
	return memcmp(a->start, b->start, a->length) == 0;
}

//scope
void markInitialized(Parser* parser) {
	parser->locals[parser->localCount - 1].depth = parser->scopeDepth;
}

void beginScope(Parser* parser) {
	parser->scopeDepth++;
}

void endScope(Parser* parser) {
	parser->scopeDepth--;

	while(parser->localCount > 0 && parser->locals[parser->localCount - 1].depth > parser->scopeDepth) {
		emitByte(parser, OP_POP); //TODO: OP_POPN
		parser->localCount--;
	}
}

//variables
void declareVariable(Parser* parser) {
	if (parser->scopeDepth == 0) return;
	Token* name = &parser->previous;

	for (int i = parser->localCount - 1; i >= 0; i--) {
		Local* local = &parser->locals[i];
		if (local->depth != -1 && local->depth < parser->scopeDepth) {
			break;
		}

		if (identifiersEqual(name, &local->name)) {
			errorAtPrevious(parser, "A variable with that name has already been declared in this scope");
		}
	}

	addLocal(parser, *name);
}

void defineVariable(Parser* parser, uint32_t global) {
	if (parser->scopeDepth > 0) {
		markInitialized(parser);
		return;
	}

	if (global < 256) {
		emitBytes(parser, OP_DEFINE_GLOBAL_VAR, global);
	} else {
		emitByte(parser, OP_DEFINE_GLOBAL_VAR_LONG);
		emitLong(parser, global);
	}
}

uint32_t parseVariable(Parser* parser, const char* errorMsg) {
	consume(parser, TOKEN_IDENTIFIER, errorMsg);

	declareVariable(parser);
	if (parser->scopeDepth > 0) return 0;

	return idenifierConstant(parser, &parser->previous);
}

void addLocal(Parser* parser, Token name) {
	//expand locals array
	if (parser->localCapacity < parser->localCount + 1) {
		int oldCapacity = parser->localCapacity;

		parser->localCapacity = GROW_CAPACITY(oldCapacity);
		parser->locals = GROW_ARRAY(Local, parser->locals, oldCapacity, parser->localCapacity);
	}

	Local* local = &parser->locals[parser->localCount++];
	local->name = name;
	local->depth = -1;
}

uint32_t resolveLocal(Parser* parser, Token* name) {
	for (int i = parser->localCount - 1; i >= 0; i--) {
		Local* local = &parser->locals[i];
		if (identifiersEqual(name, &local->name)) {
			if (local->depth == -1) {
				errorAtPrevious(parser, "Can't read variable in it's own initializer");
			}
			return i;
		}
	}

	return  -1;
}

void namedVariable(Parser* parser, Token name, bool canAssign) {
	//yes, I know there's code duplication here. Leave it alone.
	uint32_t local = resolveLocal(parser, &name);

	//handle locals
	if (local != -1) {
		if (canAssign && match(parser, TOKEN_EQUAL)) {
			expression(parser);

			if (local < 256) {
				emitBytes(parser, OP_SET_LOCAL_VAR, local);
			} else {
				emitByte(parser, OP_SET_LOCAL_VAR_LONG);
				emitLong(parser, local);
			}
		} else {
			if (local < 256) {
				emitBytes(parser, OP_GET_LOCAL_VAR, local);
			} else {
				emitByte(parser, OP_GET_LOCAL_VAR_LONG);
				emitLong(parser, local);
			}
		}
		return;
	}

	//handle globals
	uint32_t global = idenifierConstant(parser, &name);

	if (canAssign && match(parser, TOKEN_EQUAL)) {
		expression(parser);

		if (global < 256) {
			emitBytes(parser, OP_SET_GLOBAL_VAR, global);
		} else {
			emitByte(parser, OP_SET_GLOBAL_VAR_LONG);
			emitLong(parser, global);
		}
	} else {
		if (global < 256) {
			emitBytes(parser, OP_GET_GLOBAL_VAR, global);
		} else {
			emitByte(parser, OP_GET_GLOBAL_VAR_LONG);
			emitLong(parser, global);
		}
	}
}
