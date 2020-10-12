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

	//constants
	OP_LITERAL,

	//keywords
	OP_PRINT,

	//internals
	OP_POP,

	OP_LONG_SENTINEL, //everything above this opcode is long

	OP_LITERAL_LONG,
} OpCode;

#endif