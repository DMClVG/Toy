#ifndef TOY_OPCODES_H
#define TOY_OPCODES_H

/* DOCS: Remember to update opcode_names.c
*/

typedef enum {
	//end of file
	OP_EOF = 0,

	//misc. commands
	OP_RETURN,
	OP_SCOPE_BEGIN,
	OP_SCOPE_END,
	OP_GROUPING_BEGIN,
	OP_GROUPING_END,

	OP_IF_FALSE_JUMP, //if the expression on stack is false (or stack is empty) jump to a specific position

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
	OP_FUNCTION_DECLARE, //for bookkeeping
	OP_PARAMETER_DECLARE, //a variable declared as part of a function declaration, possibly

	//keywords
	OP_PRINT,

	//internals
	OP_POP,

	OP_LONG_SENTINEL, //everything above this opcode is long

	OP_LITERAL_LONG,
} OpCode;

#endif