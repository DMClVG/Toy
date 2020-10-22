#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#include "keyword_types.h"
#include "opcode_names.h"
#include "opcodes.h"

#include "function.h"

void printToken(Token* token) {
	if (token->type == TOKEN_ERROR) {
		printf("Error\t%d\t%.*s\n", token->line, token->length, token->lexeme);
		return;
	}

	printf("\t%d\t%d\t", token->type, token->line);

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

	printf("\n");
}

void printChunk(Chunk* chunk, char* prepend) {
	printf("%sPrinting chunk: %d/%d\n%s=====Opcodes=====\n", prepend, chunk->count, chunk->capacity, prepend);

	for (int i = 0; i < chunk->count; /* EMPTY */) {
		//handle literals differently
		switch(chunk->code[i]) {
			case OP_LITERAL:
				dbPrintLiteral(&chunk->literals.literals[ chunk->code[i + 1] ], prepend, false);
				i += 2;
				break;

			case OP_LITERAL_LONG:
				dbPrintLiteral(&chunk->literals.literals[ *((uint32_t*)(chunk->code + i + 1)) ], prepend, false);
				i += 5;
				break;

			default:
				printf("%s%s\n", prepend, findNameByOpCode(chunk->code[i]));
				i++;
				break;
		}
	}

	printf("%s=====Constants=====\n", prepend);
	for (int i = 0; i < chunk->literals.count; i++) {
		dbPrintLiteral(&chunk->literals.literals[i], prepend, true);
	}
}

void printChunkByteArray(Chunk* chunk) {
	for (int i = 0; i < chunk->count; i++) {
		printf("%d: %d\n", i, chunk->code[i]);
	}
}

void printLiteralArray(LiteralArray* array, char* prepend) {
	for (int i = 0; i < array->count; i++) {
		dbPrintLiteral(&array->literals[i], prepend, false);
	}
}

void printDictionary(Dictionary* dictionary) {
	//TODO: print dict
}

void dbPrintLiteral(Literal* literal, char* prepend, bool shrt) {
	switch(literal->type) {
		case LITERAL_NIL:
			printf("%snull\n", prepend);
			break;
		case LITERAL_BOOL:
			printf("%sbool (%s)\n", prepend, AS_BOOL(*literal) ? "true" : "false");
			break;
		case LITERAL_NUMBER:
			printf("%snumber (%f)\n", prepend, AS_NUMBER(*literal));
			break;
		case LITERAL_STRING:
			printf("%sstring (%s)\n", prepend, AS_STRING(*literal));
			break;
		case LITERAL_FUNCTION:
			printf("%stoy function (%d parameters)\n", prepend, AS_FUNCTION_PTR(*literal)->parameters.count);

			if (shrt) {
				break;
			}

			char p[128];
			char a[128];
			sprintf(p, "%s  ", prepend);
			sprintf(a, "%s >", prepend);

			printLiteralArray(&AS_FUNCTION_PTR(*literal)->parameters, a);
			printChunk(AS_FUNCTION_PTR(*literal)->chunk, p);
			break;
	}
}

void printScope(Scope* scope) {
	printf(" <<scope:");
	for (Scope* ptr = scope; ptr; ptr = ptr->ancestor) {
		printf("(%d) ", ptr->references);
	}
	printf(">>\n");
}