#ifndef TOY_OPCODE_NAMES_H
#define TOY_OPCODE_NAMES_H

#include "common.h"
#include "opcodes.h"

typedef struct {
	OpCode opcode;
	char* name;
} OpCodeName;

extern OpCodeName opCodeNames[];

//for debugging
char* findNameByOpCode(OpCode opcode);

#endif