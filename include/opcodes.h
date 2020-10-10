#ifndef TOY_OPCODES_H
#define TOY_OPCODES_H

typedef enum {
	//return
	OP_RETURN = 0,

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