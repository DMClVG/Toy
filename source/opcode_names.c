#include "opcode_names.h"

#include <stdio.h>

OpCodeName opCodeNames[] = {
	{OP_RETURN, "op_return"},
	{OP_SCOPE_BEGIN, "op_scope_begin"},
	{OP_SCOPE_END, "op_scope_end"},
	{OP_EQUALITY, "op_equality"},
	{OP_GREATER, "op_greater"},
	{OP_LESS, "op_less"},
	{OP_ADD, "op_add"},
	{OP_SUBTRACT, "op_subtract"},
	{OP_MULTIPLY, "op_multiply"},
	{OP_DIVIDE, "op_divide"},
	{OP_MODULO, "op_modulo"},
	{OP_NEGATE, "op_negate"},
	{OP_NOT, "op_not"},
	{OP_LITERAL, "op_literal"},
	{OP_CONSTANT_DECLARE, "op_const_decl"},
	{OP_VARIABLE_DECLARE, "op_var_decl"},
	{OP_VARIABLE_GET, "op_var_get"},
	{OP_VARIABLE_SET, "op_var_set"},

	{OP_PRINT, "op_print"},
	{OP_POP, "op_pop"},
	{OP_LONG_SENTINEL, "_sentinel"},
	{OP_LITERAL_LONG, "op_literal_long"},

	{OP_EOF, NULL}
};

char* findNameByOpCode(OpCode opcode) {
	for(int i = 0; opCodeNames[i].name; i++) {
		if (opCodeNames[i].opcode == opcode) {
			return opCodeNames[i].name;
		}
	}

	if (opcode != OP_EOF) {
		fprintf(stderr, "Found an unknown opcode in findNameByOpCode: %d\n", (int)opcode);
		return NULL;
	}

	return "op_eof";
}