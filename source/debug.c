#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#include "keyword_types.h"
#include "opcode_names.h"
#include "opcodes.h"

void printToken(Token* token) {
	if (token->type == TOKEN_ERROR) {
		printf("Error\t%d\t%.*s\n", token->line, token->length, token->lexeme);
		return;
	}

	printf("%d\t%d\t", token->type, token->line);

	if (token->type == TOKEN_IDENTIFIER || token->type == TOKEN_NUMBER || token->type == TOKEN_STRING || token->type == TOKEN_INTERPOLATED_STRING) {
		printf("%.*s\t", token->length, token->lexeme);
	} else {
		char* keyword = findKeywordByType(token->type);

		if (keyword != NULL) {
			printf("%s", keyword);
		} else {
			printf("-");
		}
	}

	printf("\t%d\n", token->line);
}

void printChunk(Chunk* chunk) {
	printf("Printing chunk: %d/%d\n\n", chunk->count, chunk->capacity);

	for (int i = 0; i < chunk->count; /* EMPTY */) {
		switch(chunk->code[i]) {
			case OP_LITERAL:
				dbPrintLiteral(&chunk->literals.literals[ chunk->code[i + 1] ]);
				i += 2;
				break;

			case OP_LITERAL_LONG:
				dbPrintLiteral(&chunk->literals.literals[ *((uint32_t*)(chunk->code + i + 1)) ]);
				i += 5;
				break;

			default:
				printf("%s\n", findNameByOpCode(chunk->code[i]));
				i++;
				break;
		}
	}

	printf("=====Constants=====\n");
	for (int i = 0; i < chunk->literals.count; i++) {
		dbPrintLiteral(&chunk->literals.literals[i]);
	}
}

void printChunkByteArray(Chunk* chunk) {
	for (int i = 0; i < chunk->count; i++) {
		printf("%d: %d\n", i, chunk->code[i]);
	}
}

void printLiteralArray(LiteralArray* literalArray) {
	//TODO: print array
}

void printDictionary(Dictionary* dictionary) {
	//TODO: print dict
}

void dbPrintLiteral(Literal* literal) {
	switch(literal->type) {
		case LITERAL_NIL:
			printf("null");
			break;
		case LITERAL_BOOL:
			printf("bool (%s)", AS_BOOL(*literal) ? "true" : "false");
			break;
		case LITERAL_NUMBER:
			printf("number (%f)", AS_NUMBER(*literal));
			break;
		case LITERAL_STRING:
			printf("string (%s)", AS_STRING(*literal));
			break;
	}

	printf("\n");
}