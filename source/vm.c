#include "vm.h"
#include "common.h"
#include "memory.h"
#include "debug.h"

#include <stdio.h>

void initVM(VM* vm) {
	vm->chunk = NULL;
	vm->ip = NULL;
	vm->capacity = 0;
	vm->count = 0;
	vm->stack = NULL;
}

void freeVM(VM* vm) {
	FREE_ARRAY(Value, vm->stack, vm->capacity);
	initVM(vm);
}

void pushVM(VM* vm, Value value) {
	if (vm->capacity < vm->count + 1) {
		int oldCapacity = vm->capacity;

		vm->capacity = GROW_CAPACITY(oldCapacity);
		vm->stack = GROW_ARRAY(Value, vm->stack, oldCapacity, vm->capacity);
	}

	vm->stack[vm->count] = value;
	vm->count++;
}

Value popVM(VM* vm) {
	//TODO: shrink stack to save memory

	vm->count--;
	return vm->stack[vm->count];
}

InterpretResult runVM(VM* vm) {
	//utility macros
#define READ_BYTE() (*vm->ip++)

#define READ_CONSTANT(opcode) \
	(opcode) == OP_CONSTANT ? \
	(vm->chunk->constants.values[READ_BYTE()]) : \
	(vm->chunk->constants.values[(*vm->ip++, *vm->ip++, *vm->ip++, *vm->ip++, *(uint32_t*)(vm->ip - 4))])

#define BINARY_OP(op, ptr) \
	do { \
		Value b = popVM(ptr); \
		Value a = popVM(ptr); \
		pushVM(ptr, a op b); \
	} while (false)

	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (int slot = 0; slot < vm->count; slot++) {
			printf("[ ");
			printValue(vm->stack[slot]);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif

		uint8_t instruction;
		switch(instruction = READ_BYTE()) {
			case OP_RETURN:
				printValue(popVM(vm));
				printf("\n");
				return INTERPRET_OK;

			case OP_CONSTANT:
			case OP_CONSTANT_LONG:
			{
				Value constant = READ_CONSTANT(instruction);
				pushVM(vm, constant);
				break;
			}

			case OP_NEGATE: pushVM(vm, -popVM(vm)); break;

			case OP_ADD:      BINARY_OP(+, vm); break;
			case OP_SUBTRACT: BINARY_OP(-, vm); break;
			case OP_MULTIPLY: BINARY_OP(*, vm); break;
			case OP_DIVIDE:   BINARY_OP(/, vm); break;

			default:
				return INTERPRET_RUNTIME_ERROR;
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}
