#include "debug.h"
#include "value.h"

#include <stdio.h>

void disassembleChunk(Chunk* chunk, char* name) {
	printf("== %s ==\n", name);

	for (int offset = 0; offset < chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

static int simpleInstruction(const char* name, int offset) {
	printf("%s\n", name);
	return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
	uint8_t constantIndex = chunk->code[offset + 1];

	printf("%-24s %d '", name, constantIndex);
	printValue(chunk->constants.values[constantIndex]);
	printf("'\n");

	return offset + 2;
}

static int constantLongInstruction(const char* name, Chunk* chunk, int offset) {
	uint32_t constantIndex = *(uint32_t*)(chunk->code + offset + 1);

	printf("%-24s %ld '", name, constantIndex);
	printValue(chunk->constants.values[constantIndex]);
	printf("'\n");

	return offset + 5;
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

		default:
			printf("Unknown opcode %d\n", opcode);
			return offset + 1;
	}
}
