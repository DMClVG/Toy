#include "debug.h"

#include "keyword_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"
#include "object.h"
#include "table.h"

void printToken(Token* token) {
	if (token->type == TOKEN_ERROR) {
		printf("Error\t%d\t%.*s\n", token->line, token->length, token->lexeme);
		return;
	}

	printf("\t%d\t%d\t", token->type, token->line);

	if (token->type == TOKEN_IDENTIFIER || token->type == TOKEN_LITERAL_INTEGER || token->type == TOKEN_LITERAL_FLOAT || token->type == TOKEN_LITERAL_STRING || token->type == TOKEN_LITERAL_STRING_INTERPOLATED) {
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


void runDebug() {
	Object* pool = NULL;
	Table storageTable;

	takeString(&pool, &storageTable, "hello world", strlen("hello world"));

	freeObjectPool(&pool);
}

//TODO: Do I really need hashes?