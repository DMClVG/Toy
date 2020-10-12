#ifndef TOY_TOKEN_TYPE_H
#define TOY_TOKEN_TYPE_H

/* DOCS: Remember to update keyword_names.c, lexer.c and the pratt table
*/

typedef enum {
	//single character tokens
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_SEMICOLON,
	TOKEN_COMMA,

	//one or two character tokens
	TOKEN_PLUS,
	TOKEN_PLUS_EQUAL,
	TOKEN_PLUS_PLUS,

	TOKEN_MINUS,
	TOKEN_MINUS_EQUAL,
	TOKEN_MINUS_MINUS,

	TOKEN_STAR,
	TOKEN_STAR_EQUAL,

	TOKEN_SLASH,
	TOKEN_SLASH_EQUAL,

	TOKEN_MODULO,
	TOKEN_MODULO_EQUAL,

	TOKEN_BANG,
	TOKEN_BANG_EQUAL,

	TOKEN_EQUAL,
	TOKEN_EQUAL_EQUAL,
	TOKEN_EQUAL_GREATER, //EQUAL_GREATER is for the arrow syntax

	TOKEN_GREATER,
	TOKEN_GREATER_EQUAL,

	TOKEN_LESS,
	TOKEN_LESS_EQUAL,
	TOKEN_LESS_OR, //backpipe

	//these can ONLY be doubles
	TOKEN_AND_AND,

	TOKEN_OR_OR,
	TOKEN_OR_GREATER, //pipe

	//these can single, double or triple
	TOKEN_DOT,
	TOKEN_DOT_DOT, //for creating events
	TOKEN_DOT_DOT_DOT, //reserved for the rest/spread operator

	//ternary operator
	TOKEN_QUESTION,
	TOKEN_COLON,

	//method operator
	TOKEN_COLON_COLON,

	//literals
	TOKEN_IDENTIFIER,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_INTERPOLATED_STRING,

	//keywords (alphabetized)
	TOKEN_AS,
	TOKEN_ASSERT,
	TOKEN_ASYNC, //reserved
	TOKEN_AWAIT, //reserved
	TOKEN_BREAK,
	TOKEN_CASE,
	TOKEN_CONST,
	TOKEN_CONTINUE,
	TOKEN_DEFAULT,
	TOKEN_DO,
	TOKEN_ELSE,
	TOKEN_EXPORT, //reserved
	TOKEN_FALSE,
	TOKEN_FOR,
	TOKEN_FOREACH, //reserved
	TOKEN_IF,
	TOKEN_IMPORT,
	TOKEN_IN, //reserved
	TOKEN_NIL, //null
	TOKEN_OF, //reserved
	TOKEN_PRINT,
	TOKEN_PURE, //reserved
	TOKEN_RETURN,
	TOKEN_SWITCH,
	TOKEN_TRUE,
	TOKEN_VAR,
	TOKEN_WHILE,

	//meta
	TOKEN_PASS, //do nothing
	TOKEN_ERROR,
	TOKEN_EOF
} TokenType;

#endif
