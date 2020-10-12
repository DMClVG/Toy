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

	array->literals[array->count] = value;
	array->count++;
}

void freeLiteralArray(LiteralArray* array) {
	//clean up memory
	for(int i = i; i < array->count; i++) {
		//TODO: clean up interpolated literals
		if (IS_STRING(array->literals[i])) {
			char* str = AS_STRING(array->literals[i]);
			FREE_ARRAY(char, str, strlen(str));
		}
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
			fprintf(stderr, "[Internal] Unrecognized literal type: %d\n", literal.type);
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

//string utilities
char* copyAndParseString(char* original, int originalLength) {
	//get the length of the new buffer
	int newLength = 0;
	for (int i = 0; original[i] && i < originalLength; i++) {
		if (original[i] == '\\') {
			i++;
		}
		newLength++;
	}

	//print each char into a new buffer
	char* buffer = ALLOCATE(char, newLength + 1);
	char* ptr = buffer;

	for (int i = 0; original[i] && i < originalLength; i++) {
		//escaped char
		if (original[i] == '\\') {
			i++;
			switch(original[i]) {
				case 'a': *ptr = '\a'; break;
				case 'b': *ptr = '\b'; break;
				case 'f': *ptr = '\f'; break;
				case 'n': *ptr = '\n'; break;
				case 'r': *ptr = '\r'; break;
				case 't': *ptr = '\t'; break;
				case 'v': *ptr = '\v'; break;
				case '\\': *ptr = '\\'; break;
				case '\'': *ptr = '\''; break;
				case '"': *ptr = '"'; break;
				case '`': *ptr = '`'; break;
			}
		} else {
			*ptr = original[i];
		}

		ptr++;
	}

	//terminate the string
	*ptr = '\0';

	return buffer;
}