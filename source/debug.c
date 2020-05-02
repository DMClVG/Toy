#include "debug.h"
#include "value.h"

#include <stdio.h>

void disassembleChunk(Chunk* chunk, char* name) {
	printf("== %s ==\n", name);

	for (int offset = 0; offset < chunk->count; /* do nothing */) {
		offset = disassembleInstruction(chunk, offset);
	}

	printf("\n");
	disassembleObjectPool(chunk->objects, "anon chunk memory");
}

static int simpleInstruction(const char* name, int offset) {
	printf("%s\n", name);
	return offset + 1;
}

static int simpleArgInstruction(const char* name, Chunk* chunk, int offset) {
	printf("%-24s %04d\n", name, chunk->code[offset + 1]);
	return offset + 2;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
	uint8_t constantIndex = chunk->code[offset + 1];

	printf("%-24s %ld '", name, constantIndex);
	printValue(chunk->constants.values[constantIndex]);
	printf("'\n");

	return offset + 2;
}

static int constantLongInstruction(const char* name, Chunk* chunk, int offset) {
	uint32_t constantIndex = *(uint32_t*)(chunk->code + offset + 1);

	printf("%-24s %ld '", name, constantIndex);
	printValue(chunk->constants.values[constantIndex]);
	printf("'\n");

	return offset + 4;
}

int disassembleInstruction(Chunk* chunk, int offset) {
	printf("%04d ", offset);

	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
		printf("   | ");
	} else {
		printf("%4d ", chunk->lines[offset]);
	}

	uint8_t opcode = chunk->code[offset];
	switch(opcode) {
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);

		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);

		case OP_CONSTANT_LONG:
			return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);

		case OP_NIL:
			return simpleInstruction("OP_NIL", offset);

		case OP_TRUE:
			return simpleInstruction("OP_TRUE", offset);
			
		case OP_FALSE:
			return simpleInstruction("OP_FALSE", offset);

		case OP_EQUAL:
			return simpleInstruction("OP_EQUAL", offset);

		case OP_GREATER:
			return simpleInstruction("OP_GREATER", offset);

		case OP_LESS:
			return simpleInstruction("OP_LESS", offset);

		case OP_NOT:
			return simpleInstruction("OP_NOT", offset);

		case OP_NEGATE:
			return simpleInstruction("OP_NEGATE", offset);

		case OP_ADD:
			return simpleInstruction("OP_ADD", offset);

		case OP_SUBTRACT:
			return simpleInstruction("OP_SUBTRACT", offset);

		case OP_MULTIPLY:
			return simpleInstruction("OP_MULTIPLY", offset);

		case OP_DIVIDE:
			return simpleInstruction("OP_DIVIDE", offset);

		case OP_PRINT:
			return simpleInstruction("OP_PRINT", offset);

		case OP_POP:
			return simpleInstruction("OP_POP", offset);

		case OP_DEFINE_GLOBAL_VAR:
			return simpleArgInstruction("OP_DEFINE_GLOBAL_VAR", chunk, offset);

		case OP_DEFINE_GLOBAL_VAR_LONG:
			return constantLongInstruction("OP_DEFINE_GLOBAL_VAR_LONG", chunk, offset);

		case OP_GET_GLOBAL:
			return constantInstruction("OP_GET_GLOBAL", chunk, offset);

		case OP_GET_GLOBAL_LONG:
			return constantLongInstruction("OP_GET_GLOBAL_LONG", chunk, offset);

			//TODO: disassemble all global variables and constants

		default:
			printf("Unknown opcode %d\n", opcode);
			return offset + 1;
	}
}

void disassembleObjectPool(Object* ptr, char* name) {
	printf("== %s ==\n", name);
	while (ptr != NULL) {
		printf("        * ");
		printObject(ptr);
		printf("\n");
		ptr = ptr->next;
	}
}