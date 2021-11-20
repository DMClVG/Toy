#include "debug.h"

#include "keyword_types.h"

#include <stdio.h>
#include <stdlib.h>

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
