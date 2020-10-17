#include "toy.h"
#include "opcodes.h"
#include "opcode_names.h"
#include "memory.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>

#define PUSH_TEMP_LITERAL pushLiteral

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

static void printStack(Toy* toy) {

	printf(" <<indexes:");

	for (int i = 0; i < toy->count; i++) {
		printf("%d;", toy->indexes[i]);
	}

	printf(">>\n");

	printf(" <<stack:");

	for (int i = 0; i < toy->count; i++) {
		printLiteral(toy->garbage.literals[toy->indexes[i]]);
		printf(";");
	}

	printf(">>\n");

	printf(" <<garbage:");

	for (int i = 0; i < toy->garbage.count; i++) {
		printLiteral(toy->garbage.literals[i]);
		printf(";");
	}

	printf(">>\n");
}

void pushLiteral(Toy* toy, Literal literal) {
	//grow the stack if necessary
	if (toy->capacity < toy->count + 1) {
		int oldCapacity = toy->capacity;

		toy->capacity = GROW_CAPACITY(oldCapacity);
		toy->indexes = GROW_ARRAY(int, toy->indexes, oldCapacity, toy->capacity);
	}

	//store the index of the literal
	writeLiteralArray(&toy->garbage, literal);
	toy->indexes[toy->count++] = toy->garbage.count - 1;
}

Literal* peekLiteral(Toy* toy) {
	//prevent underflow
	if (toy->count <= 0) {
		toy->count = 0;
		return NULL;
	}

	return &toy->garbage.literals[toy->indexes[toy->count - 1]];
}

Literal* popLiteral(Toy* toy) {
	//prevent underflow
	if (toy->count <= 0) {
		fprintf(stderr, "[No line data] Stack underflow\n");
		toy->count = 0;
		return NULL;
	}

	//grab the pointer to return
	Literal* top = &toy->garbage.literals[toy->indexes[--toy->count]];

	//finally
	return top;
}

//exposed functions
void initToy(Toy* toy) {
	toy->error = false;
	toy->capacity = 0;
	toy->count = 0;
	toy->indexes = NULL;
	toy->pc = NULL;
	initLiteralArray(&toy->garbage);
	initDictionary(&toy->constants);
	initDictionary(&toy->variables);
}

void freeToy(Toy* toy) {
	FREE_ARRAY(int, toy->indexes, toy->capacity);
	freeLiteralArray(&toy->garbage);
	freeDictionary(&toy->constants);
	freeDictionary(&toy->variables);
}

void executeChunk(Toy* toy, Chunk* chunk) {
	//NOTE: chunk MUST remain unchanged

	for (toy->pc = chunk->code; *(toy->pc) && !toy->panic; /* EMPTY */) {
		switch(*(toy->pc++)) { //TODO: change this switch to a lookup table of functions?
			//pushing && popping
			case OP_LITERAL:
				pushLiteral(toy, chunk->literals.literals[*(toy->pc)]);
				toy->pc++;
			break;

			case OP_LITERAL_LONG:
				pushLiteral(toy, chunk->literals.literals[ *((uint32_t*)(toy->pc)) ]);
				toy->pc += sizeof(uint32_t);
			break;

			case OP_POP:
				popLiteral(toy);
			break;

			case OP_PRINT: {
				Literal* top = popLiteral(toy);
				//guard against deferencing a null pointer
				if (top != NULL) {
					printLiteral(*top);
					printf("\n"); //not included in the print function above
				}
			}
			break;

			case OP_CONSTANT_DECLARE: {
				Literal* name = popLiteral(toy);
				Literal* value = popLiteral(toy);

				//check for existing constants & variables with that name
				if (!IS_NIL(dictionaryGet(&toy->constants, *name)) || !IS_NIL(dictionaryGet(&toy->variables, *name))) {
					error(toy, chunk, "Can't redefine a constant or variable");
					break;
				}

				dictionarySet(&toy->constants, *name, *value);
			}
			break;

			case OP_VARIABLE_DECLARE: {
				Literal* name = popLiteral(toy);

				//check for existing constants & variables with that name
				if (!IS_NIL(dictionaryGet(&toy->constants, *name)) || !IS_NIL(dictionaryGet(&toy->variables, *name))) {
					error(toy, chunk, "Can't redefine a constant or variable");
					break;
				}

				dictionarySet(&toy->variables, *name, TO_NIL_LITERAL); //null as default
			}
			break;

			case OP_VARIABLE_SET: {
				Literal* name = popLiteral(toy);
				Literal* value = popLiteral(toy);

				//check for existing constants & variables with that name
				if (dictionaryDeclared(&toy->constants, *name)) {
					error(toy, chunk, "Can't assign a constant");
					break;
				}

				if (!dictionaryDeclared(&toy->variables, *name)) {
					error(toy, chunk, "Can't find a variable with that name");
					break;
				}

				dictionarySet(&toy->variables, *name, *value);
			}
			break;

			case OP_VARIABLE_GET: {
				Literal* name = popLiteral(toy);

				if (dictionaryDeclared(&toy->constants, *name)) {
					PUSH_TEMP_LITERAL(toy, dictionaryGet(&toy->constants, *name));
				} else if (dictionaryDeclared(&toy->variables, *name)) {
					PUSH_TEMP_LITERAL(toy, dictionaryGet(&toy->variables, *name));
				} else {
					error(toy, chunk, "Undefined variable");
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
					break;
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
					break;
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
					break;
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
					break;
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
					break;
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
					break;
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
					break;
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
						break;
					}

					PUSH_TEMP_LITERAL(toy, TO_NUMBER_LITERAL( (long)ld % (long)rd ));
				}
				else {
					error(toy, chunk, "Mismatched types in modulo");
					break;
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
					break;
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
