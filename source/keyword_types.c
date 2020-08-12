#include "keyword_types.h"

KeywordType keywordTypes[] = {
	{"print", TOKEN_PRINT},
	//TODO: more

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