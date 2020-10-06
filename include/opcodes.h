#ifndef TOY_OPCODES_H
#define TOY_OPCODES_H

typedef enum {
	//return
	OP_RETURN = 0,

	//constants
	OP_CONSTANT,

	//keywords
	OP_PRINT,

	//internals
	OP_POP,

	OP_LONG_SENTINEL, //everything above this opcode is long

	OP_CONSTANT_LONG,
} OpCode;

#endif