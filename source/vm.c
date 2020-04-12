#include "vm.h"
#include "common.h"
#include "debug.h"

#include <stdio.h>

//TODO: make this non-static
static VM vm;

static void resetStack() {
	vm.stackTop = vm.stack;
}

void initVM() {
	resetStack();
}

void freeVM() {
	//
}

void push(Value value) {
	*vm.stackTop = value;
	vm.stackTop++;
}

Value pop() {
	vm.stackTop--;
	return *vm.stackTop;
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)

#define READ_CONSTANT(opcode) \
	(opcode) == OP_CONSTANT ? \
	(vm.chunk->constants.values[READ_BYTE()]) : \
	(vm.chunk->constants.values[(*vm.ip++, *vm.ip++, *vm.ip++, *vm.ip++, *(uint32_t*)(vm.ip - 4))])

#define BINARY_OP(op) \
	do { \
		Value b = pop(); \
		Value a = pop(); \
		push(a op b); \
	} while (false)

	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
			printf("[ ");
			printValue(*slot);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

		uint8_t instruction;
		switch(instruction = READ_BYTE()) {
			case OP_RETURN:
				printValue(pop());
				printf("\n");
				return INTERPRET_OK;

			case OP_CONSTANT:
			case OP_CONSTANT_LONG:
			{
				Value constant = READ_CONSTANT(instruction);
				push(constant);
				break;
			}

			case OP_NEGATE:
				push(-pop());
				break;

			case OP_ADD:      BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE:   BINARY_OP(/); break;

			default:
				return INTERPRET_RUNTIME_ERROR; //TODO: stack underflow?
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk) {
	vm.chunk = chunk;
	vm.ip = vm.chunk->code;
	return run();
}