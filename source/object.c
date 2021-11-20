#include "object.h"
#include "memory.h"
#include "value.h"

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

static uint32_t hashArray() {
	//TODO
	return 0;
}

static uint32_t hashDictionary() {
	//TODO
	return 0;
}

//the "constructor" for the base struct
#define ALLOCATE_OBJECT(pool, type, objectType) ((type*)allocateObject(pool, sizeof(type), objectType))

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
		case OBJECT_STRING: {
			StringObject* string = (StringObject*)object;
			FREE_ARRAY(char, string->chars, string->length + 1);
			FREE(StringObject, object);
			break;
		}

		case OBJECT_ARRAY: {
			ArrayObject* array = (ArrayObject*)object;
			freeValueArray(array->array);
			FREE(ArrayObject, object);
			break;
		}

		case OBJECT_DICTIONARY: {
			DictionaryObject* dictionary = (DictionaryObject*)object;
			freeTable(dictionary->table);
			FREE(DictionaryObject, object);
			break;
		}
	}
}

//the "constructor" for the StringObject
static StringObject* allocateString(Object** pool, Table* stringTable, char* chars, int length, uint32_t hash) {
	StringObject* object = ALLOCATE_OBJECT(pool, StringObject, OBJECT_STRING);
	object->chars = chars;
	object->length = length;
	object->object.hash = hash;
	tableSet(stringTable, &object->object, TO_NULL_VALUE()); //intern the strings
	return object;
}

StringObject* takeString(Object** pool, Table* stringTable, char* chars, int length) {
	//take ownership of the string
	uint32_t hash = hashString(chars, length);

	//allocate the string in the object pool
	return allocateString(pool, stringTable, chars, length, hash);
}

StringObject* copyString(Object** pool, Table* stringTable, const char* chars, int length) {
	uint32_t hash = hashString(chars, length);

	//copy into the heap
	char* heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';

	//allocate space in the pool
	return allocateString(pool, stringTable, heapChars, length, hash);
}

//"constructor" for the ArrayObject
ArrayObject* takeArray(Object** pool, Table* arrayTable, ValueArray* array) {
	//take ownership of the array
	uint32_t hash = hashArray(array);

	//allocate an ArrayObject for the array
	ArrayObject* object = ALLOCATE_OBJECT(pool, ArrayObject, OBJECT_ARRAY);
	object->object.hash = hash;

	tableSet(arrayTable, &object->object, TO_NULL_VALUE()); //intern the arrays
	return object;
}

//"constructor" for the DictionaryObject
DictionaryObject* takeDictionary(Object** pool, Table* dictionaryTable, Table* dictTable) {
	//take ownership of the dictionary
	uint32_t hash = hashDictionary(dictTable);

	//allocate an DictionaryObject for the array
	DictionaryObject* object = ALLOCATE_OBJECT(pool, DictionaryObject, OBJECT_DICTIONARY);
	object->object.hash = hash;

	tableSet(dictionaryTable, &object->object, TO_NULL_VALUE()); //intern the dictionaries
	return object;
}

bool objectsEqual(Object* a, Object* b) {
	if (a->type != b->type) {
		return false;
	}

	if (a->hash != b->hash) {
		return false;
	}

	switch(OBJECT_TYPE(a)) {
		case OBJECT_STRING: {
			StringObject* strA = (StringObject*)a;
			StringObject* strB = (StringObject*)b;
			return !strcmp(strA->chars, strB->chars);
			break;
		}

		default:
			return false;
	}
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
		case OBJECT_STRING: {
			StringObject* string = (StringObject*)object;
			printf("%s", string->chars);
			break;
		}

		default:
			printf("!~%s", OBJECT_TYPE(object));
	}
}