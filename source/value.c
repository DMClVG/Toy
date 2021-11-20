#include "value.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

void initValueArray(ValueArray* array) {
	array->capacity = 0;
	array->count = 0;
	array->values = NULL;
}

void writeValueArray(ValueArray* array, Value value) {
	if (array->capacity < array->count + 1) {
		int oldCapacity = array->capacity;

		array->capacity = GROW_CAPACITY(oldCapacity);
		array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
	}

	array->values[array->count] = value;
	array->count++;
}

void freeValueArray(ValueArray* array) {
	FREE_ARRAY(Value, array->values, array->capacity);
	initValueArray(array);
}

bool valuesEqual(Value a, Value b) {
	if (a.type != b.type) return false;

	switch(a.type) {
		case VALUE_NULL: return true;
		case VALUE_BOOL: return AS_BOOL(a) == AS_BOOL(b);
		case VALUE_INTEGER: return AS_INTEGER(a) == AS_INTEGER(b);
		case VALUE_FLOAT: return AS_FLOAT(a) == AS_FLOAT(b);
		case VALUE_OBJECT: return objectsEqual(AS_OBJECT(a), AS_OBJECT(b));

		default:
			return false;
	}
}

void printValue(Value value) {
	switch(value.type) {
		case VALUE_NULL: printf("null"); break;
		case VALUE_BOOL: printf(AS_BOOL(value) ? "true" : "false"); break;
		case VALUE_INTEGER: printf("%d", AS_INTEGER(value)); break;
		case VALUE_FLOAT: printf("%g", AS_FLOAT(value)); break;
		case VALUE_OBJECT: printObject(AS_OBJECT(value)); break;

		default:
			printf("~");
	}
}