#include "keyword_types.h"

#include "common.h"

KeywordType keywordTypes[] = {
	//type keywords
	{TOKEN_NULL,   "null"},
	{TOKEN_BOOL,   "bool"},
	{TOKEN_INT,    "int"},
	{TOKEN_FLOAT,  "float"},
	{TOKEN_STRING, "string"},
	{TOKEN_DICT,   "dict"},
	{TOKEN_FUNC,   "func"},

	//other keywords
	{TOKEN_PRINT,  "print"},
	{TOKEN_RETURN, "return"},
	{TOKEN_VAR,    "var"},
	{TOKEN_CONST,  "const"},

	{TOKEN_IMPORT, "import"},
	{TOKEN_EXPORT, "export"},
	{TOKEN_AS,     "as"},

	{TOKEN_WHILE,  "while"},
	{TOKEN_DO,     "do"},
	{TOKEN_FOR,    "for"},
	{TOKEN_BREAK,  "break"},
	{TOKEN_CONTINUE, "continue"},
	{TOKEN_IF,     "if"},
	{TOKEN_ELSE,   "else"},

	{TOKEN_ASSERT, "assert"},

	{TOKEN_LITERAL_TRUE,   "true"},
	{TOKEN_LITERAL_FALSE,  "false"},

	{TOKEN_EOF, NULL},
};

char* findKeywordByType(TokenType type) {
	if (type == TOKEN_EOF) {
		return "EOF";
	}

	for(int i = 0; keywordTypes[i].keyword; i++) {
		if (keywordTypes[i].type == type) {
			return keywordTypes[i].keyword;
		}
	}

	return NULL;
}