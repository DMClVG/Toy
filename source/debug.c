#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#include "keyword_types.h"

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
}

void printTable(Table* table) {
	//TODO: printTable(Table* table)
}