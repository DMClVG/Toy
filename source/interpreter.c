#include "interpreter.h"

#include "common.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

static void stdoutWrapper(const char* output) {
	fprintf(stdout, output);
	fprintf(stdout, "\n"); //default new line
}

static void stderrWrapper(const char* output) {
	fprintf(stderr, "Assertion failure: ");
	fprintf(stderr, output);
	fprintf(stderr, "\n"); //default new line
}

void initInterpreter(Interpreter* interpreter, unsigned char* bytecode, int length) {
	initLiteralArray(&interpreter->literalCache);
	interpreter->bytecode = bytecode;
	interpreter->length = length;
	interpreter->count = 0;

	initLiteralArray(&interpreter->stack);

	setInterpreterPrint(interpreter, stdoutWrapper);
	setInterpreterAssert(interpreter, stderrWrapper);
}

void freeInterpreter(Interpreter* interpreter) {
	freeLiteralArray(&interpreter->literalCache);
	FREE_ARRAY(char, interpreter->bytecode, interpreter->length);
	freeLiteralArray(&interpreter->stack);
}

//utilities for the host program
void setInterpreterPrint(Interpreter* interpreter, PrintFn printOutput) {
	interpreter->printOutput = printOutput;
}

void setInterpreterAssert(Interpreter* interpreter, PrintFn assertOutput) {
	interpreter->assertOutput = assertOutput;
}

//utils
static unsigned char readByte(unsigned char* tb, int* count) {
	unsigned char ret = *(unsigned char*)(tb + *count);
	*count += 1;
	return ret;
}

static unsigned short readShort(unsigned char* tb, int* count) {
	unsigned short ret = *(unsigned short*)(tb + *count);
	*count += 2;
	return ret;
}

static int readInt(unsigned char* tb, int* count) {
	int ret = *(int*)(tb + *count);
	*count += 4;
	return ret;
}

static float readFloat(unsigned char* tb, int* count) {
	float ret = *(float*)(tb + *count);
	*count += 4;
	return ret;
}

static char* readString(unsigned char* tb, int* count) {
	char* ret = tb + *count;
	*count += strlen(ret) + 1; //+1 for null character
	return ret;
}

static void consumeByte(unsigned char byte, unsigned char* tb, int* count) {
	if (byte != tb[*count]) {
		printf("Failed to consume the correct byte\n");
	}
	*count += 1;
}

static void consumeShort(unsigned short bytes, unsigned char* tb, int* count) {
	if (bytes != *(unsigned short*)(tb + *count)) {
		printf("Failed to consume the correct bytes\n");
	}
	*count += 2;
}

//each available statement
static bool execAssert(Interpreter* interpreter) {
	Literal rhs = popLiteralArray(&interpreter->stack);
	Literal lhs = popLiteralArray(&interpreter->stack);

	if (!IS_STRING(rhs)) {
		printf("[internal] The interpreter's assert keyword needs a string as the second argument, received: ");
		printLiteral(rhs);
		printf("\n");
		return false;
	}

	if (!IS_TRUTHY(lhs)) {
		(*interpreter->assertOutput)(AS_STRING(rhs));
		return false;
	}

	return true;
}

static bool execPrint(Interpreter* interpreter) {
	//print what is on top of the stack, then pop it
	Literal lit = popLiteralArray(&interpreter->stack);

	printLiteralCustom(lit, interpreter->printOutput);

	freeLiteral(lit);

	return true;
}

static bool execPushLiteral(Interpreter* interpreter, bool lng) {
	//read the index in the cache
	int index = 0;

	if (lng) {
		index = (int)readShort(interpreter->bytecode, &interpreter->count);
	}
	else {
		index = (int)readByte(interpreter->bytecode, &interpreter->count);
	}

	//push from cache to stack
	pushLiteralArray(&interpreter->stack, interpreter->literalCache.literals[index]);

	return true;
}

static bool execNegate(Interpreter* interpreter) {
	//negate the top literal on the stack
	Literal lit = popLiteralArray(&interpreter->stack);

	if (IS_INTEGER(lit)) {
		lit = TO_INTEGER_LITERAL(-AS_INTEGER(lit));
	}
	else if (IS_FLOAT(lit)) {
		lit = TO_FLOAT_LITERAL(-AS_FLOAT(lit));
	}
	else {
		printf("[internal] The interpreter can't negate that literal: ");
		printLiteral(lit);
		printf("\n");
		return false;
	}

	pushLiteralArray(&interpreter->stack, lit);
	return true;
}

static bool execArithmetic(Interpreter* interpreter, Opcode opcode) {
	Literal rhs = popLiteralArray(&interpreter->stack);
	Literal lhs = popLiteralArray(&interpreter->stack);

	//type coersion
	if (IS_FLOAT(lhs) && IS_INTEGER(rhs)) {
		rhs = TO_FLOAT_LITERAL(AS_INTEGER(rhs));
	}

	if (IS_INTEGER(lhs) && IS_FLOAT(rhs)) {
		lhs = TO_FLOAT_LITERAL(AS_INTEGER(lhs));
	}

	//maths based on types
	if(IS_INTEGER(lhs) && IS_INTEGER(rhs)) {
		switch(opcode) {
			case OP_ADDITION:
				pushLiteralArray(&interpreter->stack, TO_INTEGER_LITERAL( AS_INTEGER(lhs) + AS_INTEGER(rhs) ));
				return true;

			case OP_SUBTRACTION:
				pushLiteralArray(&interpreter->stack, TO_INTEGER_LITERAL( AS_INTEGER(lhs) - AS_INTEGER(rhs) ));
				return true;

			case OP_MULTIPLICATION:
				pushLiteralArray(&interpreter->stack, TO_INTEGER_LITERAL( AS_INTEGER(lhs) * AS_INTEGER(rhs) ));
				return true;

			case OP_DIVISION:
				pushLiteralArray(&interpreter->stack, TO_INTEGER_LITERAL( AS_INTEGER(lhs) / AS_INTEGER(rhs) ));
				return true;

			case OP_MODULO:
				pushLiteralArray(&interpreter->stack, TO_INTEGER_LITERAL( AS_INTEGER(lhs) % AS_INTEGER(rhs) ));
				return true;

		}
	}

	//catch bad modulo
	if (opcode == OP_MODULO) {
		printf("Bad arithmetic argument (modulo on floats not allowed)\n");
		return false;
	}

	if(IS_FLOAT(lhs) && IS_FLOAT(rhs)) {
		switch(opcode) {
			case OP_ADDITION:
				pushLiteralArray(&interpreter->stack, TO_FLOAT_LITERAL( AS_FLOAT(lhs) + AS_FLOAT(rhs) ));
				return true;

			case OP_SUBTRACTION:
				pushLiteralArray(&interpreter->stack, TO_FLOAT_LITERAL( AS_FLOAT(lhs) - AS_FLOAT(rhs) ));
				return true;

			case OP_MULTIPLICATION:
				pushLiteralArray(&interpreter->stack, TO_FLOAT_LITERAL( AS_FLOAT(lhs) * AS_FLOAT(rhs) ));
				return true;

			case OP_DIVISION:
				pushLiteralArray(&interpreter->stack, TO_FLOAT_LITERAL( AS_FLOAT(lhs) / AS_FLOAT(rhs) ));
				return true;
		}
	}

	//wrong types
	printf("Bad arithmetic argument\n");
	return false;
}

//the heart of toy
static void execInterpreter(Interpreter* interpreter) {
	unsigned char opcode = readByte(interpreter->bytecode, &interpreter->count);

	while(opcode != OP_EOF && opcode != OP_SECTION_END) {
		switch(opcode) {
			case OP_ASSERT:
				if (!execAssert(interpreter)) {
					return;
				}
			break;

			case OP_PRINT:
				if (!execPrint(interpreter)) {
					return;
				}
			break;

			case OP_LITERAL:
			case OP_LITERAL_LONG:
				if (!execPushLiteral(interpreter, opcode == OP_LITERAL_LONG)) {
					return;
				}
			break;

			case OP_NEGATE:
				if (!execNegate(interpreter)) {
					return;
				}
			break;

			case OP_ADDITION:
			case OP_SUBTRACTION:
			case OP_MULTIPLICATION:
			case OP_DIVISION:
			case OP_MODULO:
				if (!execArithmetic(interpreter, opcode)) {
					return;
				}
			break;

			case OP_GROUPING_BEGIN:
				execInterpreter(interpreter);
			break;

			case OP_GROUPING_END:
				return;

			default:
				printf("Unknown opcode found %d, terminating\n", opcode);
				printLiteralArray(&interpreter->stack, "\n");
				return;
		}

		opcode = readByte(interpreter->bytecode, &interpreter->count);
	}
}

void runInterpreter(Interpreter* interpreter) {
	//header section
	const unsigned char major = readByte(interpreter->bytecode, &interpreter->count);
	const unsigned char minor = readByte(interpreter->bytecode, &interpreter->count);
	const unsigned char patch = readByte(interpreter->bytecode, &interpreter->count);
	const char* build = readString(interpreter->bytecode, &interpreter->count);

	if (command.verbose) {
		if (major != TOY_VERSION_MAJOR || minor != TOY_VERSION_MINOR || patch != TOY_VERSION_PATCH) {
			printf("Warning: interpreter/bytecode version mismatch\n");
		}

		if (!strncmp(build, TOY_VERSION_BUILD, strlen(TOY_VERSION_BUILD))) {
			printf("Warning: interpreter/bytecode build mismatch\n");
		}
	}

	consumeByte(OP_SECTION_END, interpreter->bytecode, &interpreter->count);

	//data section
	const short literalCount = readShort(interpreter->bytecode, &interpreter->count);

	if (command.verbose) {
		printf("Reading %d literals\n", literalCount);
	}

	for (int i = 0; i < literalCount; i++) {
		const unsigned char literalType = readByte(interpreter->bytecode, &interpreter->count);

		switch(literalType) {
			case LITERAL_NULL:
				//read the null
				pushLiteralArray(&interpreter->literalCache, TO_NULL_LITERAL);

				if (command.verbose) {
					printf("(null)\n");
				}
			break;

			case LITERAL_BOOLEAN: {
				//read the booleans
				const bool b = readByte(interpreter->bytecode, &interpreter->count);
				pushLiteralArray(&interpreter->literalCache, TO_BOOLEAN_LITERAL(b));

				if (command.verbose) {
					printf("(boolean %s)\n", b ? "true" : "false");
				}
			}
			break;

			case LITERAL_INTEGER: {
				const int d = readInt(interpreter->bytecode, &interpreter->count);
				pushLiteralArray(&interpreter->literalCache, TO_INTEGER_LITERAL(d));

				if (command.verbose) {
					printf("(integer %d)\n", d);
				}
			}
			break;

			case LITERAL_FLOAT: {
				const float f = readFloat(interpreter->bytecode, &interpreter->count);
				pushLiteralArray(&interpreter->literalCache, TO_FLOAT_LITERAL(f));

				if (command.verbose) {
					printf("(float %f)\n", f);
				}
			}
			break;

			case LITERAL_STRING: {
				char* s = readString(interpreter->bytecode, &interpreter->count);
				pushLiteralArray(&interpreter->literalCache, TO_STRING_LITERAL(s));

				if (command.verbose) {
					printf("(string \"%s\")\n", s);
				}
			}
			break;
		}
	}

	consumeByte(OP_SECTION_END, interpreter->bytecode, &interpreter->count);

	//code section
	if (command.verbose) {
		printf("executing bytecode\n");
	}

	execInterpreter(interpreter);
}
