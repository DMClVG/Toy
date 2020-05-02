#include "object.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

//utility functions
static uint32_t hashString(const char* key, int length) {
	uint32_t hash = 2166136261u;

	for (int i = 0; i < length; i++) {
		hash *= key[i];
		hash *= 16777619;
	}

	return hash;
}

//the "constructor" for the base struct
#define ALLOCATE_OBJECT(pool, type, objectType) (type*)allocateObject(pool, sizeof(type), objectType)

static Object* allocateObject(Object** pool, size_t size, ObjectType type) {
	Object* object = (Object*)reallocate(NULL, 0, size);
	object->type = type;

	//track the objects for memory
	object->next = *pool;
	*pool = object;

	return object;
}

static void freeObject(Object* object) {
	switch(object->type) {
		case OBJ_STRING: {
			ObjectString* string = (ObjectString*)object;
			FREE_ARRAY(char, string->chars, string->length + 1);
			FREE(ObjectString, object);
			break;
		}
	}
}

//the "constructor" for the string object
static ObjectString* allocateString(Object** pool, Table* stringTable, char* chars, int length, uint32_t hash) {
	ObjectString* string = ALLOCATE_OBJECT(pool, ObjectString, OBJ_STRING);
	string->chars = chars;
	string->length = length;
	string->hash = hash;
	tableSet(stringTable, string, NIL_VAL); //intern the strings
	return string;
}

ObjectString* takeString(Object** pool, Table* stringTable, char* chars, int length) {
	//take ownership of the string
	uint32_t hash = hashString(chars, length);

	//get the interned string instead
	ObjectString* interned = tableFindString(stringTable, chars, length, hash);
	if (interned != NULL) {
		FREE_ARRAY(char, chars, length + 1);
		return interned;
	}

	return allocateString(pool, stringTable, chars, length, hash);
}

ObjectString* copyString(Object** pool, Table* stringTable, const char* chars, int length) {
	uint32_t hash = hashString(chars, length);

	//get the interned string instead
	ObjectString* interned = tableFindString(stringTable, chars, length, hash);
	if (interned != NULL) return interned;

	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';

	return allocateString(pool, stringTable, heapChars, length, hash);
}

//utilities
void freeObjectPool(Object** pool) {
	while (*pool != NULL) {
		Object* prev = *pool;
		*pool = (*pool)->next;
		freeObject(prev);
	}
}

Object* mergeObjectPools(Object* a, Object* b) {
	Object* top = a;

	if (a != NULL) {
		while(a->next != NULL) {
			a = a->next;
		}

		a->next = b;
	}

	return top;
}

void printObject(Object* object) {
	switch(OBJECT_TYPE(object)) {
		case OBJ_STRING: {
			ObjectString* string = (ObjectString*)object;
			printf("%s", string->chars);
			break;
		}

		default:
			printf("!~%s", OBJECT_TYPE(object));
	}
}
