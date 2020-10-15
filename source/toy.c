#include "toy.h"
#include "opcodes.h"
#include "opcode_names.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

static void error(Toy* toy, Chunk* chunk, const char* message) {
	//TODO: option to strip debug data (lines) from the chunks
	if (chunk->lines) {
		fprintf(stderr, "[Line %d] Runtime Error: %s\n", chunk->lines[(int)toy->pc - (int)chunk->code - 1], message);
	} else {
		fprintf(stderr, "[Line data stripped] Runtime Error: %s\n", message);
	}

	toy->error = true;
	toy->panic = true;
}

void pushLiteral(Toy* toy, Literal* literal) {
	//grow the stack if necessary
	if (toy->capacity < toy->count + 1) {
		int oldCapacity = toy->capacity;

		toy->capacity = GROW_CAPACITY(oldCapacity);
		toy->stack = GROW_ARRAY(Literal*, toy->stack, oldCapacity, toy->capacity);
	}

	//store the pointer to the literal (literals live in chunks)
	toy->stack[toy->count++] = literal;
}

Literal* peekLiteral(Toy* toy) {
	//prevent underflow
	if (toy->count <= 0) {
		toy->count = 0;
		return NULL;
	}

	return toy->stack[toy->count - 1];
}

Literal* popLiteral(Toy* toy) {
	//prevent underflow
	if (toy->count <= 0) {
		fprintf(stderr, "[No line data] Stack underflow\n");
		toy->count = 0;
		return NULL;
	}

	//grab the pointer to return
	Literal* top = toy->stack[toy->count - 1];

	//lose the reference to the literal, and decrement the counter
	toy->stack[toy->count--] = NULL;

	//finally
	return top;
}

//exposed functions
void initToy(Toy* toy) {
	toy->error = false;
	toy->capacity = 0;
	toy->count = 0;
	toy->stack = NULL;
	toy->pc = NULL;
	initLiteralArray(&toy->garbage);
}

void freeToy(Toy* toy) {
	FREE_ARRAY(Literal*, toy->stack, toy->capacity);
	freeLiteralArray(&toy->garbage);
	initToy(toy);
}

void executeChunk(Toy* toy, Chunk* chunk) {
	//NOTE: chunk MUST remain unchanged

	for (toy->pc = chunk->code; *(toy->pc) && !toy->panic; /* EMPTY */) {
		switch(*(toy->pc++)) {
			//pushing && popping
			case OP_LITERAL:
				pushLiteral(toy, &chunk->literals.literals[*(toy->pc)]);
				toy->pc++;
				break;

			case OP_LITERAL_LONG:
				pushLiteral(toy, &chunk->literals.literals[ *((uint32_t*)(toy->pc)) ]); //TODO: test long literals
				toy->pc += sizeof(uint32_t);
				break;

			case OP_POP:
				popLiteral(toy);
				break;

			case OP_PRINT: {
				//guard against deferencing a null pointer
				Literal* top = peekLiteral(toy);
				if (top != NULL) {
					printLiteral(*top);
					printf("\n"); //not included in the function above
					popLiteral(toy);
				}
			}
			break;

			//operations
			case OP_EQUALITY: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_BOOL_LITERAL(AS_NUMBER(*lhs) == AS_NUMBER(*rhs)));
				} else

				if (IS_STRING(*lhs) && IS_STRING(*rhs)) {
					//the garbage array now gains ownership of this c-string
					PUSH_TEMP_LITERAL(toy, TO_BOOL_LITERAL( strcmp(AS_STRING(*lhs), AS_STRING(*rhs)) == 0 ));
				}

				else {
					error(toy, chunk, "Mismatched types in equality");
				}
			}
			break;

			case OP_GREATER: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_BOOL_LITERAL(AS_NUMBER(*lhs) > AS_NUMBER(*rhs)));
				}
				else {
					error(toy, chunk, "Mismatched types in comparison");
				}
			}
			break;

			case OP_LESS: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_BOOL_LITERAL(AS_NUMBER(*lhs) < AS_NUMBER(*rhs)));
				}
				else {
					error(toy, chunk, "Mismatched types in comparison");
				}
			}
			break;

			case OP_ADD: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL(AS_NUMBER(*lhs) + AS_NUMBER(*rhs)));
				} else

				if (IS_STRING(*lhs) && IS_STRING(*rhs)) {
					//allocate a buffer
					int bufSize = STRLEN(*lhs) + STRLEN(*rhs);
					char* buffer = ALLOCATE(char, bufSize + 1);

					strcpy(buffer, AS_STRING(*lhs));
					strcpy(buffer + STRLEN(*lhs), AS_STRING(*rhs));

					//the garbage array now gains ownership of this c-string
					PUSH_TEMP_LITERAL(toy, TO_STRING_LITERAL( buffer ));
				}

				else {
					error(toy, chunk, "Mismatched types in addition");
				}
			}
			break;

			case OP_SUBTRACT: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL(AS_NUMBER(*lhs) - AS_NUMBER(*rhs)));
				}
				else {
					error(toy, chunk, "Mismatched types in subtraction");
				}
			}
			break;

			case OP_MULTIPLY: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL(AS_NUMBER(*lhs) * AS_NUMBER(*rhs)));
				}
				else {
					error(toy, chunk, "Mismatched types in multiplication");
				}
			}
			break;

			case OP_DIVIDE: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*lhs) && IS_NUMBER(*rhs)) {
					//write the resulting literal to the garbage array, then push it to the stack
					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL(AS_NUMBER(*lhs) / AS_NUMBER(*rhs)));
				}
				else {
					error(toy, chunk, "Mismatched types in division");
				}
			}
			break;

			case OP_MODULO: {
				Literal* rhs = popLiteral(toy);
				Literal* lhs = popLiteral(toy);

				if (IS_NUMBER(*rhs) && IS_NUMBER(*lhs)) {
					double rd = AS_NUMBER(*rhs);
					double ld = AS_NUMBER(*lhs);

					//determine if there's any fractional parts
					if (rd - (long)rd != 0 || ld - (long)ld != 0) {
						error(toy, chunk, "Operands to modulo must be whole numbers");
					}

					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL( (long)ld % (long)rd ));
				}
				else {
					error(toy, chunk, "Mismatched types in modulo");
				}
			}
			break;

			case OP_NEGATE: {
				Literal* rhs = popLiteral(toy);

				if (IS_NUMBER(*rhs)) {
					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL(-AS_NUMBER(*rhs)));
				}
				else {
					error(toy, chunk, "Unknown type in negation");
				}
			}
			break;

			case OP_NOT: {
				Literal* rhs = popLiteral(toy);

				//TODO: (optimisation) literal reuse, just like in the parser
				if (IS_TRUTHY(*rhs)) {
					PUSH_TEMP_LITERAL(toy, TO_BOOL_LITERAL(false));
				} else {
					PUSH_TEMP_LITERAL(toy, TO_BOOL_LITERAL(true));
				}
			}
			break;

			default:
				fprintf(stderr, "[Internal] Unexpected OpCode in Toy virtual machine %s\n", findNameByOpCode(*(toy->pc-1)));
				toy->error = true;
				break;
		}
	}
}
