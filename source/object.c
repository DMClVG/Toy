#include "object.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

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
static ObjectString* allocateString(Object** pool, char* chars, int length) {
	ObjectString* string = ALLOCATE_OBJECT(pool, ObjectString, OBJ_STRING);
	string->chars = chars;
	string->length = length;
	return string;
}

ObjectString* takeString(Object** pool, char* chars, int length) {
	//take ownership of the string
	return allocateString(pool, chars, length);
}

ObjectString* copyString(Object** pool, const char* chars, int length) {
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';

	return allocateString(pool, heapChars, length);
}

//utilities
void freeObjectPool(Object** pool) {
	while (*pool != NULL) {
		Object* prev = *pool;
		*pool = (*pool)->next;
		freeObject(prev);
	}
}

void printObject(Object* object) {
	switch(OBJECT_TYPE(object)) {
		case OBJ_STRING: {
			ObjectString* string = (ObjectString*)object;
			printf("%s", string->chars);
			break;
		}
	}
}
