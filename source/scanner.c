#include "scanner.h"
#include "common.h"

#include <string.h>

//independant helpers
static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

//scanner-dependant helpers
static bool isAtEnd(Scanner* scanner) {
	return *scanner->current == '\0';
}

static char peek(Scanner* scanner) {
	return *scanner->current;
}

static char peekNext(Scanner* scanner) {
	if (isAtEnd(scanner)) return '\0';
	return scanner->current[1];
}

static char advance(Scanner* scanner) {
	scanner->current++;
	return scanner->current[-1];
}

static bool match(Scanner* scanner, char expected) {
	if (isAtEnd(scanner)) return false;
	if (*scanner->current != expected) return false;

	scanner->current++;
	return true;
}

static void eatWhitespace(Scanner* scanner) {
	for (;;) {
		char c = peek(scanner);
		switch(c) {
			case ' ':
			case '\r':
			case '\t':
				advance(scanner);
				break;

			//newlines
			case '\n':
				scanner->line++;
				advance(scanner);
				break;

			//comments
			case '/':
				if (peekNext(scanner) == '/') {
					while(peek(scanner) != '\n' && !isAtEnd(scanner)) advance(scanner);
					break;
				}

				if (peekNext(scanner) == '*') {
					advance(scanner);
					advance(scanner);
					while (!(peek(scanner) == '*' && peekNext(scanner) == '/')) advance(scanner);
					advance(scanner);
					advance(scanner);
					break;
				}

			default:
				return;
		}
	}
}

//token makers
static Token makeToken(Scanner* scanner, TokenType type) {
	Token token;

	token.type = type;
	token.start = scanner->start;
	token.length = (int)(scanner->current - scanner->start);
	token.line = scanner->line;

	return token;
}

static Token makeErrorToken(Scanner* scanner, const char* message) {
	Token token;

	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int)strlen(message);
	token.line = scanner->line;

	return token;
}

static Token makeString(Scanner* scanner) {
	while (peek(scanner) != '"' && !isAtEnd(scanner)) {
		if (peek(scanner) == '\n') scanner->line++;
		advance(scanner);
	}

	if (isAtEnd(scanner)) return makeErrorToken(scanner, "Unterminated string");

	advance(scanner);
	return makeToken(scanner, TOKEN_STRING);
}

static Token makeNumber(Scanner* scanner) {
	while(isDigit(peek(scanner))) advance(scanner);

	//fractions
	if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
		advance(scanner);
	}

	while(isDigit(peek(scanner))) advance(scanner);

	return makeToken(scanner, TOKEN_NUMBER);
}

//for processing keywords efficiently
static TokenType checkKeyword(Scanner* scanner, int start, int length, const char* rest, TokenType type) {
	if (scanner->current - scanner->start == start + length && memcmp(scanner->start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static TokenType makeIdentifierType(Scanner* scanner) {
	//TODO: maybe a proper trie would be useful here afterall
	switch(scanner->start[0]) {
		case 'a':
			if (scanner->current - scanner->start > 1) { //longer than 1 character
				switch(scanner->start[1]) {
					case 's':
						//handle as and assert
						if (scanner->current - scanner->start > 2) {
							return checkKeyword(scanner, 2, 4, "sert", TOKEN_ASSERT);
						}
						//just "as"
						return TOKEN_AS;
				}
			}
		break;

		case 'b':
			return checkKeyword(scanner, 1, 4, "reak", TOKEN_BREAK);

		case 'c':
			if (scanner->current - scanner->start > 1) { //longer than 1 character
				switch(scanner->start[1]) {
					case 'o':
						//handle const and continue
						if (scanner->current - scanner->start > 2) { //longer than 2 characters
							switch(scanner->start[2]) {
								case 'n':
									//handle const and continue (again)
									if (scanner->current - scanner->start > 3) {
										switch(scanner->start[3]) {
											case 's': return checkKeyword(scanner, 4, 1, "t", TOKEN_CONST);
											case 't': return checkKeyword(scanner, 4, 4, "inue", TOKEN_CONTINUE);
										}
									}
								break; //n
							}
						}
					break; //o
				}
			}
		break;

		case 'd':
			return checkKeyword(scanner, 1, 1, "o", TOKEN_DO);

		case 'e':
			return checkKeyword(scanner, 1, 3, "lse", TOKEN_ELSE);

		case 'f':
			if (scanner->current - scanner->start > 1) { //longer than 1 character
				switch(scanner->start[1]) {
					case 'a':
						return checkKeyword(scanner, 2, 3, "lse", TOKEN_FALSE);
					case 'o':
						if (scanner->current - scanner->start > 2) {
							switch(scanner->start[2]) {
								case 'r':
									//handle for and foreach
									if (scanner->current - scanner->start > 3) {
										return checkKeyword(scanner, 3, 4, "each", TOKEN_FOREACH);
									}

									return TOKEN_FOR;
								break;
							}
						}
					break;
				}
			}
		break;

		case 'i':
			if (scanner->current - scanner->start > 1) { //longer than 1 character
				switch(scanner->start[1]) {
					case 'f': return checkKeyword(scanner, 2, 0, "", TOKEN_IF);
					case 'm': return checkKeyword(scanner, 2, 4, "port", TOKEN_IMPORT);
					case 'n': return checkKeyword(scanner, 2, 0, "", TOKEN_IN);
				}
			}
		break;

		case 'n': return checkKeyword(scanner, 1, 3, "ull", TOKEN_NIL);
		case 'o': return checkKeyword(scanner, 1, 1, "f", TOKEN_OF);
		case 'p': return checkKeyword(scanner, 1, 4, "rint", TOKEN_PRINT);
		case 'r': return checkKeyword(scanner, 1, 5, "eturn", TOKEN_RETURN);
		case 't': return checkKeyword(scanner, 1, 3, "rue", TOKEN_TRUE);
		case 'v': return checkKeyword(scanner, 1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(scanner, 1, 4, "hile", TOKEN_WHILE);
	}

	return TOKEN_IDENTIFIER;
}

static Token makeIdentifier(Scanner* scanner) {
	while (isAlpha(peek(scanner)) || isDigit(peek(scanner))) advance(scanner);
	return makeToken(scanner, makeIdentifierType(scanner));
}

//exposed functions
void initScanner(Scanner* scanner, const char* source) {
	scanner->start = source;
	scanner->current = source;
	scanner->line = 1;
}

//scan function
Token scanToken(Scanner* scanner) {
	eatWhitespace(scanner);

	scanner->start = scanner->current;

	if (isAtEnd(scanner)) return makeToken(scanner, TOKEN_EOF);

	char c = advance(scanner);

	if (isAlpha(c)) return makeIdentifier(scanner);
	if (isDigit(c)) return makeNumber(scanner);

	switch(c) {
		case '(': return makeToken(scanner, TOKEN_LEFT_PAREN);
		case ')': return makeToken(scanner, TOKEN_RIGHT_PAREN);
		case '[': return makeToken(scanner, TOKEN_LEFT_BRACE);
		case ']': return makeToken(scanner, TOKEN_RIGHT_BRACE);
		case '{': return makeToken(scanner, TOKEN_LEFT_BRACE);
		case '}': return makeToken(scanner, TOKEN_RIGHT_BRACE);
		case ';': return makeToken(scanner, TOKEN_SEMICOLON);
		case ',': return makeToken(scanner, TOKEN_COMMA);

		case '+': return makeToken(scanner, match(scanner, '=') ? TOKEN_PLUS_EQUAL : match(scanner, '+') ? TOKEN_PLUS_PLUS : TOKEN_PLUS);
		case '-': return makeToken(scanner, match(scanner, '=') ? TOKEN_MINUS_EQUAL : match(scanner, '-') ? TOKEN_MINUS_MINUS : TOKEN_MINUS);
		case '*': return makeToken(scanner, match(scanner, '=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
		case '/': return makeToken(scanner, match(scanner, '=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
		case '%': return makeToken(scanner, match(scanner, '=') ? TOKEN_MODULO_EQUAL : TOKEN_MODULO);

		case '!': return makeToken(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=': return makeToken(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : match(scanner, '>') ? TOKEN_EQUAL_GREATER : TOKEN_EQUAL);

		case '>': return makeToken(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '<': return makeToken(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : ('|') ? TOKEN_LESS_OR : TOKEN_LESS);

		case '&':
			if (match(scanner, '&')) return makeToken(scanner, TOKEN_AND_AND);
		break;

		case '|':
			if (match(scanner, '|')) return makeToken(scanner, TOKEN_OR_OR);
			if (match(scanner, '>')) return makeToken(scanner, TOKEN_OR_GREATER);
		break;

		case '.':
			//three-parts
			if (peek(scanner) == '.' && peekNext(scanner) == '.') {
				advance(scanner);
				advance(scanner);
				return makeToken(scanner, TOKEN_DOT_DOT_DOT);
			}

			return makeToken(scanner, TOKEN_DOT);

		case '?': return makeToken(scanner, TOKEN_QUESTION);
		case ':': return makeToken(scanner, TOKEN_COLON);

		case '"': return makeString(scanner);
	}

	return makeErrorToken(scanner, "Unexpected character");
}
