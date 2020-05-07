#include "grammar_rules.h"
#include "parser.h"
#include "parser_tools.h"
#include "token_rules.h"

//gammar rules
static void varDecl(Parser* parser) {
	uint32_t var = parseVariable(parser, "Expected variable name");

	if (match(parser, TOKEN_EQUAL)) {
		expression(parser);
	} else {
		emitByte(parser, OP_NIL); //all values default to 'null'
	}

	consume(parser, TOKEN_SEMICOLON, "Expected semicolon after variable declaration");

	defineVariable(parser, var);
}

static void printStmt(Parser* parser) {
	expression(parser);
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of print statement");
	emitByte(parser, OP_PRINT);
}

static void expressionStmt(Parser* parser) {
	expression(parser);
	consume(parser, TOKEN_SEMICOLON, "Expected ';' at end of expression statement");
	emitByte(parser, OP_POP);
}

static void block(Parser* parser) {
	while (!check(parser, TOKEN_RIGHT_BRACE) && !check(parser, TOKEN_EOF)) {
		declaration(parser);
	}

	consume(parser, TOKEN_RIGHT_BRACE, "Expected '}' at the end of a block");
}

//exposed
void declaration(Parser* parser) {
	if (match(parser, TOKEN_VAR)) {
		varDecl(parser);
	} else { //TODO: const
		statement(parser);
	}

	if (parser->panicMode) {
		synchronize(parser);
	}
}

void statement(Parser* parser) {
	//print
	if (match(parser, TOKEN_PRINT)) {
		printStmt(parser);
		return;
	}

	//block
	if (match(parser, TOKEN_LEFT_BRACE)) {
		beginScope(parser);
		block(parser);
		endScope(parser);
		return;
	}

	//default
	expressionStmt(parser);
}

void expression(Parser* parser) {
	parsePrecendence(parser, PREC_ASSIGNMENT);
}
