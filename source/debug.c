#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#include "keyword_types.h"
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

	printf("\n", token->line);
}

void printChunk(Chunk* chunk) {
	//TODO: printChunk(Chunk* chunk)
	printf("Printing chunk: %d/%d\n\n", chunk->count, chunk->capacity);

	for (int i = 0; i < chunk->count; /* EMPTY */) {
		switch(chunk->code[i]) {
			case OP_PRINT: {
				if (chunk->code[i + 1] == OP_LITERAL) {
					printf("print literal %u\n", chunk->code[i + 2]);
					i += 3;
				}

				else if (chunk->code[i + 1] == OP_LITERAL_LONG) {
					printf("print literal-long %d\n", *((uint32_t*)(chunk->code + i + 2)));
					i += 6;
				}

				else {
					printf("Unexpected OP passed to OP_PRINT\n");
					return;
				}
			}

			break;

			case OP_RETURN:
				printf("return\n");
				i++;
				break;

			default:
				printf("Unexpected OP passed to printChunk %d\n", chunk->code[i]);
				return;
		}
	}

	printf("=====Constants=====\n");
	for (int i = 0; i < chunk->literals.count; i++) {
		switch(chunk->literals.literals[i].type) {
			case LITERAL_NIL:
				printf("null\n");
				break;
			case LITERAL_BOOL:
				printf("bool (%s)\n", AS_BOOL(chunk->literals.literals[i]) ? "true" : "false");
				break;
			case LITERAL_NUMBER:
				printf("number (%d)\n", AS_NUMBER(chunk->literals.literals[i]));
				break;
			case LITERAL_STRING:
				printf("string (%s)\n", AS_STRING(chunk->literals.literals[i]));
				break;
		}
	}
}

void printTable(Table* table) {
	//TODO: printTable(Table* table)
}