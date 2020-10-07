#include "keyword_types.h"

KeywordType keywordTypes[] = {
	{"as", TOKEN_AS},
	{"assert", TOKEN_ASSERT},
	{"async", TOKEN_ASYNC},
	{"await", TOKEN_AWAIT},
	{"break", TOKEN_BREAK},
	{"case", TOKEN_CASE},
	{"const", TOKEN_CONST},
	{"continue", TOKEN_CONTINUE},
	{"default", TOKEN_DEFAULT},
	{"do", TOKEN_DO},
	{"else", TOKEN_ELSE},
	{"export", TOKEN_EXPORT},
	{"false", TOKEN_FALSE},
	{"for", TOKEN_FOR},
	{"foreach", TOKEN_FOREACH},
	{"if", TOKEN_IF},
	{"import", TOKEN_IMPORT},
	{"in", TOKEN_IN},
	{"null", TOKEN_NIL},
	{"of", TOKEN_OF},
	{"print", TOKEN_PRINT},
	{"pure", TOKEN_PURE},
	{"return", TOKEN_RETURN},
	{"switch", TOKEN_SWITCH},
	{"true", TOKEN_TRUE},
	{"var", TOKEN_VAR},
	{"while", TOKEN_WHILE},

	{NULL, TOKEN_EOF}
};

char* findKeywordByType(TokenType type) {
	for(int i = 0; keywordTypes[i].keyword; i++) {
		if (keywordTypes[i].type == type) {
			return keywordTypes[i].keyword;
		}
	}

	return NULL;
}