#include "keyword_types.h"

#include "common.h"

KeywordType keywordTypes[] = {
	//type keywords
	{TOKEN_NULL,       "null"},
	{TOKEN_BOOLEAN,    "bool"},
	{TOKEN_INTEGER,    "int"},
	{TOKEN_FLOAT,      "float"},
	{TOKEN_STRING,     "string"},
	{TOKEN_ARRAY,      "array"},
	{TOKEN_DICTIONARY, "dictionary"},
	{TOKEN_FUNCTION,   "function"},
	{TOKEN_ANY,        "any"},

	//other keywords
	{TOKEN_AS,         "as"},
	{TOKEN_ASSERT,     "assert"},
	{TOKEN_BREAK,      "break"},
	{TOKEN_CLASS,      "class"},
	{TOKEN_CONST,      "const"},
	{TOKEN_CONTINUE,   "continue"},
	{TOKEN_DO,         "do"},
	{TOKEN_ELSE,       "else"},
	{TOKEN_EXPORT,     "export"},
	{TOKEN_FOR,        "for"},
	{TOKEN_FOREACH,    "foreach"},
	{TOKEN_IF,         "if"},
	{TOKEN_IMPORT,     "import"},
	{TOKEN_IN,         "in"},
	{TOKEN_OF,         "of"},
	{TOKEN_PRINT,      "print"},
	{TOKEN_RETURN,     "return"},
	{TOKEN_USING,      "using"},
	{TOKEN_VAR,        "var"},
	{TOKEN_WHILE,      "while"},

	//literal values
	{TOKEN_LITERAL_TRUE,   "true"},
	{TOKEN_LITERAL_FALSE,  "false"},

	//meta tokens
	{TOKEN_PASS,       "pass"},
	{TOKEN_ERROR,      "error"},

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