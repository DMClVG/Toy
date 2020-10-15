#include "dictionary.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//"don't use modulo" - some gamedev guy

//utility functions
static uint32_t hashString(const char* string, int length) {
	uint32_t hash = 2166136261u;

	for (int i = 0; i < length; i++) {
		hash *= string[i];
		hash *= 16777619;
	}

	return hash;
}

//entry functions
void setEntry(Entry* dest, Literal* key, Literal* value) {
	//keys
	if (IS_STRING(*key)) {
		//copy the string memory for the key
		int len = STRLEN(*key);
		char* buffer = ALLOCATE(char, len + 1);
		strcpy(buffer, AS_STRING(*key));
		buffer[len] = '\0';
		dest->key = TO_STRING_LITERAL(buffer);
	} else {
		dest->key = *key; //not reachable
	}

	//values
	if (IS_STRING(*value)) {
		//copy the string memory for the key
		int len = STRLEN(*value);
		char* buffer = ALLOCATE(char, len + 1);
		strcpy(buffer, AS_STRING(*value));
		buffer[len] = '\0';
		dest->value = TO_STRING_LITERAL(buffer);
	} else {
		dest->value = *value;
	}
}

void freeEntry(Entry* entry) {
	//TODO: handle interpolated strings?
	if (IS_STRING(entry->key)) {
		FREE_ARRAY(char, AS_STRING(entry->key), STRLEN(entry->key) + 1);
	}

	if (IS_STRING(entry->value)) {
		FREE_ARRAY(char, AS_STRING(entry->value), STRLEN(entry->value) + 1);
	}
	entry->key = TO_NIL_LITERAL;
	entry->value = TO_NIL_LITERAL;
}

void freeEntryArray(Entry* array, int capacity) {
	if (array == NULL) {
		return;
	}

	for (int i = 0; i < capacity; i++) {
		freeEntry(&array[i]);
	}

	FREE_ARRAY(Entry, array, capacity);
}

Entry* entryArrayGet(Entry* array, int capacity, Literal key, int startPos) {
	//just in case
	int index = startPos % capacity;

	//literal probing and collision checking
	for (;;) { //WARNING: this is the only function allowed to retrieve an entry from the array
		Entry* entry = &array[index];

		if (IS_NIL(entry->key)) { //if key is empty, it's either empty or tombstone
			if (IS_NIL(entry->value)) {
				//found a truly empty bucket
				return entry;
			}
			//else it's a tombstone - ignore
		} else {
			if (IS_STRING(key) && IS_STRING(entry->key) && strcmp(AS_STRING(key), AS_STRING(entry->key)) == 0) {
				return entry;
			}
		}

		index = (index + 1) % capacity;
	}
}

Entry* adjustCapacity(Entry* array, int oldCapacity, int capacity) {
	//new entries
	Entry* newEntries = ALLOCATE(Entry, capacity);

	for (int i = 0; i < capacity; i++) {
		newEntries[i].key = TO_NIL_LITERAL;
		newEntries[i].value = TO_NIL_LITERAL;
	}

	//move the old array into the new one
	for (int i = 0; i < oldCapacity; i++) {
		if (IS_NIL(array[i].key)) {
			continue;
		}

		Entry* pos = NULL;

		if (IS_STRING(array[i].key)) {
			//grab a nil key at X
			pos = entryArrayGet(newEntries, capacity, TO_NIL_LITERAL, hashString(AS_STRING(array[i].key), STRLEN(array[i].key)) % capacity);
		}

		//place the key and value in the new array (reusing string memory)
		pos->key = array[i].key;
		pos->value = array[i].value;
	}

	//clear the old array
	FREE_ARRAY(Entry, array, oldCapacity);

	return newEntries;
}

bool entryArraySet(Entry** array, int* capacityPtr, int count, double load, Literal key, Literal value, int startPos) {
	//expand array
	if (count + 1 > *capacityPtr * load) {
		int oldCapacity = *capacityPtr;
		*capacityPtr = GROW_CAPACITY(*capacityPtr);
		*array = adjustCapacity(*array, oldCapacity, *capacityPtr); //custom rather than automatic reallocation

		//recalc start pos
		if (IS_STRING(key)) {
			startPos = hashString(AS_STRING(key), STRLEN(key)) % *capacityPtr;
		} else

		{
			fprintf(stderr, "[Internal] Couldn't recalc startPos\n");
			return false;
		}
	}

	Entry* entry = entryArrayGet(*array, *capacityPtr, key, startPos);

	//true = count increase
	if (IS_NIL(entry->key)) {
		setEntry(entry, &key, &value);
		return true;
	} else {
		setEntry(entry, &key, &value);
		return false;
	}
}

//init & free
void initDictionary(Dictionary* dict) {
	//HACK: because modulo by 0 is undefined, set the capacity to a non-zero value (and allocate the arrays)
	dict->capacity = GROW_CAPACITY(0);
	dict->count = 0;
	dict->entries = adjustCapacity(NULL, 0, dict->capacity);

	dict->load = DICTIONARY_MAX_LOAD;
}

void freeDictionary(Dictionary* dict) {
	freeEntryArray(dict->entries, dict->capacity);
}

//accessors & mutators
Literal dictionaryGet(Dictionary* dict, Literal key) {
	if (IS_STRING(key)) {
		return entryArrayGet(dict->entries, dict->capacity, key, hashString(AS_STRING(key), STRLEN(key)) % dict->capacity)->value;
	} else

	{
		fprintf(stderr, "[Internal] Couldn't get that key's type in a dictionary\n");
		return TO_NIL_LITERAL;
	}

	//TODO: interpolated strings
}

void dictionarySet(Dictionary* dict, Literal key, Literal value) {
	if (IS_STRING(key)) {
		if (entryArraySet(&dict->entries, &dict->capacity, dict->count, dict->load, key, value, hashString(AS_STRING(key), STRLEN(key)) % dict->capacity)) {
			dict->count++;
		}
	} else

	{
		fprintf(stderr, "[Internal] Couldn't set that key's type in a dictionary\n");
		return;
	}

	//TODO: interpolated strings
}

void dictionaryDelete(Dictionary* dict, Literal key) {
	if (IS_STRING(key)) {
		Entry* entry = entryArrayGet(dict->entries, dict->capacity, key, hashString(AS_STRING(key), STRLEN(key)) % dict->capacity);
		freeEntry(entry);
		entry->value = TO_BOOL_LITERAL(true);//tombstone
	} else

	{
		fprintf(stderr, "[Internal] Couldn't delete that key's type in a dictionary\n");
		return;
	}
}

bool dictionaryDeclared(Dictionary* dict, Literal key) {
	if (IS_STRING(key)) {
		Entry* entry = entryArrayGet(dict->entries, dict->capacity, key, hashString(AS_STRING(key), STRLEN(key)) % dict->capacity);
		return !IS_NIL(entry->key);
	} else

	{
		fprintf(stderr, "[Internal] Couldn't query that key's type in a dictionary\n");
		return false;
	}
}

//important
void dictionaryCopy(Dictionary* target, Dictionary* source) {
	for (int i = 0; i < source->capacity; i++) {
		if (!IS_NIL(source->entries[i].key)) {
			dictionarySet(target, source->entries[i].key, source->entries[i].value);
		}
	}
}
