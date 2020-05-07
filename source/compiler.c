#include "compiler.h"
#include "scanner.h"
#include "parser.h"
#include "parser_tools.h"
#include "grammar_rules.h"
#include "debug.h"

//exposed
bool compile(const char* source, Chunk* chunk) {
	//TODO: return bytecode for running or saving to a file
	Scanner scanner;
	Parser parser;

	//init scanner & parser
	initScanner(&scanner, source);
	initParser(&parser, &scanner, chunk);

	//process
	advance(&parser);
	while (!match(&parser, TOKEN_EOF)) {
		declaration(&parser);
	}

	//return
	emitByte(&parser, OP_RETURN);

#ifdef DEBUG_PRINT_CODE
	if (!parser.hadError) {
		disassembleChunk(chunk, "code");
	}
#endif

	bool hadError = parser.hadError;
	freeParser(&parser);

	return !hadError;
}