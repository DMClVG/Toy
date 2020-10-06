#include "literal.h"
#include "memory.h"

#include <stdio.h>

void initLiteralArray(LiteralArray* array) {
	array->capacity = 0;
	array->count = 0;
	array->literals = NULL;
}

void writeLiteralArray(LiteralArray* array, Literal value) {
	if (array->capacity < array->count + 1) {
		int oldCapacity = array->capacity;

		array->capacity = GROW_CAPACITY(oldCapacity);
		array->literals = GROW_ARRAY(Literal, array->literals, oldCapacity, array->capacity);
	}

	array->literals[array->count] = value;
	array->count++;
}

void freeLiteralArray(LiteralArray* array) {
	FREE_ARRAY(Literal, array->literals, array->capacity);
	initLiteralArray(array);
}

void printLiteral(Literal literal) {
	switch(literal.type) {
		case LITERAL_NIL:
			printf("null");
			break;

		case LITERAL_BOOL:
			printf(AS_BOOL(literal) ? "true" : "false");
			break;

		case LITERAL_NUMBER:
			printf("%g", AS_NUMBER(literal));
			break;

		case LITERAL_STRING:
			printf("%s", AS_STRING(literal));
			break;

		default:
			printf("~");
	}
}
