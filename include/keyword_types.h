#ifndef TOY_KEYWORD_TYPES_H_
#define TOY_KEYWORD_TYPES_H_

#include "token_type.h"

typedef struct {
	TokenType type;
	char* keyword;
} KeywordType;

extern KeywordType keywordTypes[];

//for debugging
char* findKeywordByType(TokenType type);

#endif //TOY_KEYWORD_TYPES_H_
