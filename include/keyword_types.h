#ifndef TOY_KEYWORD_TYPES_H
#define TOY_KEYWORD_TYPES_H

#include "common.h"
#include "token_type.h"

typedef struct {
	char* keyword;
	TokenType type;
} KeywordType;

extern KeywordType keywordTypes[];

char* findKeywordByType(TokenType type);

#endif