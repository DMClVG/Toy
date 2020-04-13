#include "scanner.h"
#include "common.h"

#include <string.h>

typedef struct {
	const char* start;
	const char* current;
	int line;
} Scanner;

Scanner scanner; //TODO: move this

//helpers
static bool isAtEnd() {
	return *scanner.current == '\0';
}

static Token makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

static Token makeErrorToken(const char* message) {
	Token token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner.line;
	return token;
}

static char advance() {
	scanner.current++;
	return scanner.current[-1];
}

static char peek() {
	return *scanner.current;
}

static char peekNext() {
	if (isAtEnd()) return '\0';
	return scanner.current[1];
}

static bool match(char expected) {
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;

	scanner.current++;
	return true;
}

static void eatWhitespace() {
	for (;;) {
		char c = peek();
		switch(c) {
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;

			//newlines
			case '\n':
				scanner.line++;
				advance();
				break;

			//comments
			case '/': //TODO: multiline comments
				if (peekNext() == '/') {
					while(peek() != '\n' && !isAtEnd()) advance();
				} else {
					return;
				}
				break;

			default:
				return;
		}
	}
}

static Token makeString() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') scanner.line++;
		advance();
	}

	if (isAtEnd()) return makeErrorToken("Unterminated string");

	advance();
	return makeToken(TOKEN_STRING);
}

static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

static Token makeNumber() {
	while(isDigit(peek())) advance();

	//fractions
	if (peek() == '.' && isDigit(peekNext())) {
		advance();
	}

	while(isDigit(peek())) advance();

	return makeToken(TOKEN_NUMBER);
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
	if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static TokenType makeIdentifierType() {
	switch(scanner.start[0]) {
//		case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
//		case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'f':
			if (scanner.current - scanner.start > 1) {
				switch(scanner.start[1]) {
					case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
				}
			}
			break;
		//TODO: more
	}

	return TOKEN_IDENTIFIER;
}

static Token makeIdentifier() {
	while (isAlpha(peek()) || isDigit(peek())) advance();
	return makeToken(makeIdentifierType());
}


void initScanner(const char* source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

//scan function
Token scanToken() {
	eatWhitespace();

	scanner.start = scanner.current;

	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();

	if (isAlpha(c)) return makeIdentifier();
	if (isDigit(c)) return makeNumber();

	switch(c) {
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);

		case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

		case '"': return makeString();
	}

	return makeErrorToken("Unexpected character");
}
