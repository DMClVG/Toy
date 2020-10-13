#ifndef TOY_OPCODES_H
#define TOY_OPCODES_H

/* DOCS: Remember to update opcode_names.c
*/

typedef enum {
	//end of file
	OP_EOF = 0,

	//return
	OP_RETURN,

	//operations
	OP_EQUALITY,
	OP_GREATER,
	OP_LESS,

	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_MODULO,

	OP_NEGATE,
	OP_NOT,

	//data
	OP_LITERAL,
	OP_CONSTANT_DECLARE,
	OP_VARIABLE_DECLARE,
	OP_VARIABLE_GET,
	OP_VARIABLE_SET,

	//keywords
	OP_PRINT,

	//internals
	OP_POP,

	OP_LONG_SENTINEL, //everything above this opcode is long

	OP_LITERAL_LONG,
} OpCode;

#endif