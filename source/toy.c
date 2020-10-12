#include "toy.h"
#include "opcodes.h"
#include "opcode_names.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

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
		fprintf(stderr, "Stack underflow\n");
		toy->count = 0;
		return NULL;
	}

	return toy->stack[toy->count - 1];
}

Literal* popLiteral(Toy* toy) {
	//prevent underflow
	if (toy->count <= 0) {
		fprintf(stderr, "Stack underflow\n");
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
	initLiteralArray(&toy->garbage);
}

void freeToy(Toy* toy) {
	FREE_ARRAY(Literal*, toy->stack, toy->capacity);
	freeLiteralArray(&toy->garbage);
	initToy(toy);
}

void executeChunk(Toy* toy, Chunk* chunk) {
	//NOTE: chunk MUST remain unchanged

	for (uint8_t* pc = chunk->code; *pc && !toy->error; /* EMPTY */) {
		switch(*(pc++)) {
			//pushing && popping
			case OP_LITERAL:
				pushLiteral(toy, &chunk->literals.literals[*pc]);
				pc++;
				break;

			case OP_LITERAL_LONG:
				pushLiteral(toy, &chunk->literals.literals[ *((uint32_t*)pc) ]); //TODO: test long literals
				pc += sizeof(uint32_t);
				break;

			case OP_POP:
				popLiteral(toy);
				break;

			case OP_PRINT:
				printLiteral(*peekLiteral(toy));
				printf("\n"); //not included in the function above
				popLiteral(toy);
				break;

			//operations
			case OP_EQUALITY:
				//TODO
				break;

			case OP_GREATER:
				//TODO
				break;

			case OP_LESS:
				//TODO
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
					int bufSize = strlen(AS_STRING(*lhs)) + strlen(AS_STRING(*rhs));
					char* buffer = ALLOCATE(char, bufSize + 1);

					strcpy(buffer, AS_STRING(*lhs));
					strcpy(buffer + strlen( AS_STRING(*lhs) ), AS_STRING(*rhs));

					//the garbage array now gains ownership of this c-string
					PUSH_TEMP_LITERAL(toy, TO_STRING_LITERAL( buffer ));
				}

				else {
					fprintf(stderr, "Mismatched types in addition\n"); //TODO: proper error handling with chunk->lines
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
					fprintf(stderr, "Mismatched types in subtraction\n"); //TODO: proper error handling with chunk->lines
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
					fprintf(stderr, "Mismatched types in multiplication\n"); //TODO: proper error handling with chunk->lines
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
					fprintf(stderr, "Mismatched types in division\n"); //TODO: proper error handling with chunk->lines
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
						fprintf(stderr, "operands to modulo must be whole numbers\n"); //TODO: proper error handling with chunk->lines
					}

					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL( (long)ld % (long)rd ));
				}
				else {
					fprintf(stderr, "Mismatched types in modulo\n"); //TODO: proper error handling with chunk->lines
				}
			}
			break;

			case OP_NEGATE: {
				Literal* rhs = popLiteral(toy);

				if (IS_NUMBER(*rhs)) {
					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL(-AS_NUMBER(*rhs)));
				}
				else {
					fprintf(stderr, "Unknown type in negation\n"); //TODO: proper error handling with chunk->lines
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
				fprintf(stderr, "Unexpected OpCode in Toy virtual machine %s\n", findNameByOpCode(*(pc-1)));
				toy->error = true;
				break;
		}
	}
}
