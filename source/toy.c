#include "toy.h"
#include "opcodes.h"
#include "opcode_names.h"
#include "memory.h"
#include "debug.h"

#include "function.h"

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
	/*
	printf(" <<indexes:");

	for (int i = 0; i < toy->count; i++) {
		printf("%d;", toy->indexes[i]);
	}

	printf(">>\n");
	*/

	printf(" <<stack:");

	for (int i = 0; i < toy->count; i++) {
		printLiteral(toy->garbage.literals[toy->indexes[i]]);
		printf(";");
	}

	printf(">>\n");

	/*
	printf(" <<garbage:");

	for (int i = 0; i < toy->garbage.count; i++) {
		printLiteral(toy->garbage.literals[i]);
		printf(";");
	}

	printf(">>\n");
	*/
}

static void pushLiteral(Toy* toy, Literal literal) {
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

static Literal* peekLiteral(Toy* toy) {
	//prevent underflow
	if (toy->count <= 0) {
		toy->count = 0;
		return NULL;
	}

	return &toy->garbage.literals[toy->indexes[toy->count - 1]];
}

static Literal* popLiteral(Toy* toy) {
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

//TODO: could use int for multiple returns
static int loopOverChunk(Toy* toy, Chunk* chunk, int groupingDepth) { //NOTE: chunk MUST remain unchanged
	while (*(toy->pc) && !toy->panic) {

//		printStack(toy); //debugging

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

			case OP_RETURN:
				return 1;
			break;

			case OP_SCOPE_BEGIN:
				toy->scope = pushScope(toy->scope);
			break;

			case OP_SCOPE_END:
				toy->scope = popScope(toy->scope);
			break;

			case OP_GROUPING_BEGIN: {
				//grab any functions on top of the stack
				Function* func = NULL;

				if (peekLiteral(toy) != NULL && IS_FUNCTION(*peekLiteral(toy))) {
					func = AS_FUNCTION_PTR(*peekLiteral(toy)); //leave it on the stack as a sentinel
				}

				//use C's stack frame to read the insides of the grouping
				loopOverChunk(toy, chunk, groupingDepth + 1);

				//process and execute the function
				if (func != NULL) {
					//push a new scope onto the function
					func->scope = pushScope(func->scope);

					//pop each value from the stack and pass in as a parameter (backwards)
					for (int i = func->parameters.count - 1; i >= 0; i--) {
						Literal* arg = popLiteral(toy);

						if (IS_FUNCTION(*arg) && AS_FUNCTION_PTR(*arg) == func) {
							error(toy, chunk, "Too few parameters found");
							break;
						}

						//the arg needs to be given the name at thisChunk->literals(parameters[i])
						if (!scopeSetVariable(func->scope, func->parameters.literals[func->parameters.count - i - 1], *arg, true)) {
							error(toy, chunk, "Can't reuse function parameters");
						}
					}

					if (toy->panic) { //double loops
						break;
					}

					//check the sentinel pointer
					if (!(peekLiteral(toy) != NULL && IS_FUNCTION(*peekLiteral(toy)) && func == AS_FUNCTION_PTR(*peekLiteral(toy)))) {
						error(toy, chunk, "Too many parameters found");

						printLiteral(*peekLiteral(toy));
						break;
					}

					popLiteral(toy); //pop the function now

					//switch in the new function scope
					Scope* tmpScopePtr = toy->scope;
					toy->scope = func->scope;

					//switch the pc for the new chunk pc
					uint8_t* pc = toy->pc;
					toy->pc = func->chunk->code;

					//finally, call the function
					loopOverChunk(toy, func->chunk, groupingDepth + 1);

					// //begin reverting what was done
					toy->scope = tmpScopePtr;
					toy->pc = pc;

					func->scope = popScope(func->scope);

					//any returned value should be on the stack
				}
			}
			break;

			case OP_GROUPING_END: {
				return 0;
			}
			break;

			//keywords
			case OP_PRINT: {
				Literal* top = popLiteral(toy);
				//guard against deferencing a null pointer
				if (top != NULL) {
					printLiteral(*top);
					printf("\n"); //not included in the print function above
				}
			}
			break;

			//handle variables
			case OP_CONSTANT_DECLARE: {
				Literal* name = popLiteral(toy);
				Literal* value = popLiteral(toy);

				if (!scopeSetConstant(toy->scope, *name, *value, true)) {
					error(toy, chunk, "Can't redefine a constant or variable");
					break;
				}
			}
			break;

			case OP_VARIABLE_DECLARE: {
				Literal* name = popLiteral(toy);

				if (!scopeSetVariable(toy->scope, *name, TO_NIL_LITERAL, true)) {
					error(toy, chunk, "Can't redefine a constant or variable");
					break;
				}
			}
			break;

			case OP_VARIABLE_SET: {
				Literal* name = popLiteral(toy);
				Literal* value = popLiteral(toy);

				if (!scopeSetVariable(toy->scope, *name, *value, false)) {
					error(toy, chunk, "Must assign to a variable that has been declared (not a constant)");
					break;
				}
			}
			break;

			case OP_VARIABLE_GET: {
				Literal* name = popLiteral(toy);
				bool defined = true;

				Literal lit = scopeGet(toy->scope, *name, &defined);

				if (!defined) {
					error(toy, chunk, "Undefined variable");
					break;
				} else {
					PUSH_TEMP_LITERAL(toy, lit);
				}
			}
			break;

			case OP_FUNCTION_DECLARE: {
				//store a reference to the current scope in the pointer
				AS_FUNCTION_PTR(*peekLiteral(toy))->scope = referenceScope(toy->scope); //is it possible to combine this with the push opcodes?

				//TODO: pure functions?
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

					//BUFGIX: plug a hole
					FREE_ARRAY(char, buffer, bufSize + 1);
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

	return 0;
}

//exposed functions
void initToy(Toy* toy) {
	toy->error = false;
	toy->panic = false;
	toy->capacity = 0;
	toy->count = 0;
	toy->indexes = NULL;
	toy->pc = NULL;
	initLiteralArray(&toy->garbage);
	toy->scope = pushScope(NULL);
}

void freeToy(Toy* toy) {
	FREE_ARRAY(int, toy->indexes, toy->capacity);
	freeLiteralArray(&toy->garbage);
	toy->scope = popScope(toy->scope);
}

void executeChunk(Toy* toy, Chunk* chunk) {
	toy->pc = chunk->code;
	loopOverChunk(toy, chunk, 0);
}