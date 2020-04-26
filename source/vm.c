#include "vm.h"
#include "common.h"
#include "memory.h"
#include "compiler.h"
#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

//for errors
static void runtimeError(VM* vm, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm->ip - vm->chunk->code - 1;
	int line = vm->chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);

	//TODO: free the stack?
}

//VM control
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

Value peekVM(VM* vm, int distance) {
	if (distance >= vm->count) {
		runtimeError(vm, "Can't peek that deep into the stack: %d / %d", distance, vm->count);
		return NIL_VAL;
	}

	return vm->stack[vm->count - 1 - distance];
}

static bool isFalsy(Value value) {
	//falsy: null, false and 0
	return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value)) || (IS_NUMBER(value) && AS_NUMBER(value) == 0);
}

InterpretResult runVM(VM* vm) {
	//utility macros
#define READ_BYTE() (*vm->ip++)

#define READ_CONSTANT(opcode) \
	(opcode) == OP_CONSTANT ? \
	(vm->chunk->constants.values[READ_BYTE()]) : \
	(vm->chunk->constants.values[(*vm->ip++, *vm->ip++, *vm->ip++, *vm->ip++, *(uint32_t*)(vm->ip - 4))])

#define BINARY_OP(valueType, op, ptr) \
	do { \
		if (!IS_NUMBER(peekVM(ptr, 0)) || !IS_NUMBER(peekVM(ptr, 1))) { \
			runtimeError(ptr, "Operands must be numbers"); \
			return INTERPRET_RUNTIME_ERROR; \
		} \
		double b = AS_NUMBER(popVM(ptr)); \
		double a = AS_NUMBER(popVM(ptr)); \
		pushVM(ptr, valueType(a op b)); \
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

			case OP_NOT:
				pushVM(vm, BOOL_VAL(isFalsy(popVM(vm))));
				break;

			case OP_NEGATE:
				if (!IS_NUMBER(peekVM(vm, 0))) {
					runtimeError(vm, "Operand must be a number");
					return INTERPRET_RUNTIME_ERROR;
				}

				pushVM(vm, NUMBER_VAL(-AS_NUMBER(popVM(vm))));
				break;

			case OP_ADD:      BINARY_OP(NUMBER_VAL, +, vm); break;
			case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -, vm); break;
			case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *, vm); break;
			case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /, vm); break;

			case OP_TRUE: pushVM(vm, BOOL_VAL(true)); break;
			case OP_FALSE: pushVM(vm, BOOL_VAL(false)); break;

			case OP_NIL: pushVM(vm, NIL_VAL); break;

			case OP_EQUAL: {
				Value b = popVM(vm);
				Value a = popVM(vm);

				pushVM(vm, BOOL_VAL(valuesEqual(a, b)));
				break;
			}

			case OP_GREATER: BINARY_OP(BOOL_VAL, >, vm); break;
			case OP_LESS:    BINARY_OP(BOOL_VAL, <, vm); break;

			default:
				return INTERPRET_RUNTIME_ERROR;
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpretVM(VM* vm, const char* source) {
	//TODO: compile and run separately
	Chunk chunk;
	initChunk(&chunk);

	if (!compile(source, &chunk)) {
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm->chunk = &chunk;
	vm->ip = vm->chunk->code;

	InterpretResult result = runVM(vm);

	freeChunk(&chunk);
	return result;
}