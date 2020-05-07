#include "parser.h"
#include "memory.h"

//exposed functions
void initParser(Parser* parser, Scanner* scanner, Chunk* chunk) {
	parser->scanner = scanner;
	parser->chunk = chunk;

	parser->locals = NULL;
	parser->localCapacity = 0;
	parser->localCount = 0;
	parser->scopeDepth = 0;

	parser->hadError = false;
	parser->panicMode = false;
}

void freeParser(Parser* parser) {
	FREE_ARRAY(Local, parser->locals, parser->localCapacity);
	initParser(parser, NULL, NULL);
}
