#include "literal.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

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

	//TODO: interpolated strings

	//if it's a string
	if (IS_STRING(value)) {
		int len = STRLEN(value);
		char* buffer = ALLOCATE(char, len + 1);
		strcpy(buffer, AS_STRING(value));
		buffer[len] = '\0';
		value = TO_STRING_LITERAL(buffer);
	}

	array->literals[array->count++] = value;
}

void freeLiteralArray(LiteralArray* array) {
	//clean up memory
	for(int i = 0; i < array->count; i++) {
		//TODO: clean up interpolated literals
		freeLiteral(array->literals[i]);
	}

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
			//should never bee seen
			fprintf(stderr, "[Internal] Unrecognized literal type: %d", literal.type);
	}
}

//find a literal in the array that matches the "literal" argument
int findLiteral(LiteralArray* array, Literal literal) {
	for (int i = 0; i < array->count; i++) {
		//not the same type
		if (array->literals[i].type != literal.type) {
			continue;
		}

		//matching type, compare values
		switch(array->literals[i].type) {
			case LITERAL_NIL:
				return i;

			case LITERAL_BOOL:
				if (AS_BOOL(array->literals[i]) == AS_BOOL(literal)) {
					return i;
				}
				break;

			case LITERAL_NUMBER:
				if (AS_NUMBER(array->literals[i]) == AS_NUMBER(literal)) {
					return i;
				}
				break;

			case LITERAL_STRING:
				if (strcmp(AS_STRING(array->literals[i]), AS_STRING(literal)) == 0) {
					return i;
				}
				break;

			default:
				fprintf(stderr, "Unexpected literal type in findLiteral: %d\n", literal.type);
				break;
		}
	}

	return -1;
}

void freeLiteral(Literal literal) {
	//TODO: clean up interpolated literals
	if (IS_STRING(literal)) {
		FREE_ARRAY(char, AS_STRING(literal), STRLEN(literal) + 1);
	}
}

Literal _toStringLiteral(char* cstr) {
	return ((Literal){LITERAL_STRING, { .string.ptr = (char*)cstr, .string.length = strlen((char*)cstr) }});
}