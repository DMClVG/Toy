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
		int len = strlen(AS_STRING(*key));
		char* buffer = ALLOCATE(char, len + 1);
		strcpy(buffer, AS_STRING(*key));
		buffer[len] = '\0';
		dest->key = TO_STRING_LITERAL(buffer);
	} else {
		dest->key = *key;
	}

	//values
	if (IS_STRING(*value)) {
		//copy the string memory for the key
		int len = strlen(AS_STRING(*value));
		char* buffer = ALLOCATE(char, len + 1);
		strcpy(buffer, AS_STRING(*value));
		buffer[len] = '\0';
		dest->value = TO_STRING_LITERAL(buffer);
	} else {
		dest->value = *value;
	}
}

void freeEntry(Entry* entry) {
	//TODO: handle interpolated strings
	if (IS_STRING(entry->key)) {
		FREE_ARRAY(char, AS_STRING(entry->key), strlen( AS_STRING(entry->key) ) + 1);
	}

	if (IS_STRING(entry->value)) {
		FREE_ARRAY(char, AS_STRING(entry->value), strlen( AS_STRING(entry->value) ) + 1);
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
			//we found a filled bucket
			if (IS_NUMBER(key) && IS_NUMBER(entry->key) && AS_NUMBER(key) == AS_NUMBER(entry->key)) {
				return entry;
			}

			if (IS_STRING(key) && IS_STRING(entry->key) && strcmp(AS_STRING(key), AS_STRING(entry->key)) == 0) {
				return entry;
			}
		}

		index = (index + 1) % capacity;
	}
}

Entry* adjustCapacity(Entry* array, int oldCapacity, int capacity) {
	//new entries
	Entry* newEntries = ALLOCATE(Entry, capacity); //ERROR: this line fails

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

		if (IS_NUMBER(array[i].key)) {
			pos = entryArrayGet(newEntries, capacity, TO_NIL_LITERAL, ((int)AS_NUMBER(array[i].key)) % capacity);
		}

		if (IS_STRING(array[i].key)) {
			pos = entryArrayGet(newEntries, capacity, TO_NIL_LITERAL, hashString(AS_STRING(array[i].key), strlen(AS_STRING(array[i].key))) % capacity);
		}

		//place the key and value in the new array (reusing string memory)
		pos->key = array[i].key;
		pos->value = array[i].value;
	}

	//clear the old array
	FREE_ARRAY(Entry, array, oldCapacity);

	return newEntries;
}

bool entryArraySet(Entry** array, int* capacityPtr, int count, Literal key, Literal value, int startPos) {
	//expand array
	if (count + 1 > *capacityPtr * DICTIONARY_MAX_LOAD) {
		int oldCapacity = *capacityPtr;
		*capacityPtr = GROW_CAPACITY(*capacityPtr);
		*array = adjustCapacity(*array, oldCapacity, *capacityPtr); //custom rather than automatic reallocation

		//recalc start pos
		if (IS_NUMBER(key)) {
			startPos = ((int)AS_NUMBER(key)) % *capacityPtr;
		}
		if (IS_STRING(key)) {
			startPos = hashString(AS_STRING(key), strlen(AS_STRING(key))) % *capacityPtr;
		}
	}

	Entry* entry = entryArrayGet(*array, *capacityPtr, key, startPos);

	//TODO: count increase
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
	dict->nilEntry.key = TO_NIL_LITERAL;
	dict->nilEntry.value = TO_NIL_LITERAL;
	dict->trueEntry.key = TO_NIL_LITERAL;
	dict->trueEntry.value = TO_NIL_LITERAL;
	dict->falseEntry.key = TO_NIL_LITERAL;
	dict->falseEntry.value = TO_NIL_LITERAL;

	//HACK: because modulo by 0 is undefined, set the capacity to a non-zero value (and allocate the arrays)

	dict->numberCapacity = GROW_CAPACITY(0);
	dict->numberCount = 0;
	dict->numberEntries = adjustCapacity(NULL, 0, dict->numberCapacity);

	dict->stringCapacity = GROW_CAPACITY(0);
	dict->stringCount = 0;
	dict->stringEntries = adjustCapacity(NULL, 0, dict->stringCapacity);
}

void freeDictionary(Dictionary* dict) {
	freeEntry(&dict->nilEntry);
	freeEntry(&dict->trueEntry);
	freeEntry(&dict->falseEntry);

	freeEntryArray(dict->numberEntries, dict->numberCapacity);
	freeEntryArray(dict->stringEntries, dict->stringCapacity);
}

//accessors & mutators
Literal dictionaryGet(Dictionary* dict, Literal key) {
	if (IS_NIL(key)) {
		return dict->nilEntry.value;
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == true) {
		return dict->trueEntry.value;
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == false) {
		return dict->falseEntry.value;
	} else

	if (IS_NUMBER(key)) {
		return entryArrayGet(dict->numberEntries, dict->numberCapacity, key, ((int)AS_NUMBER(key)) % dict->numberCapacity)->value;
	} else

	if (IS_STRING(key)) {
		return entryArrayGet(dict->stringEntries, dict->stringCapacity, key, hashString(AS_STRING(key), strlen(AS_STRING(key))) % dict->stringCapacity)->value;
	} else

	{
		fprintf(stderr, "[Internal]Couldn't find that key's type in a dictionary\n");
		exit(-1);
	}

	//TODO: interpolated strings
}

void dictionarySet(Dictionary* dict, Literal key, Literal value) {
	if (IS_NIL(key)) {
		setEntry(&dict->nilEntry, &key, &value);
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == true) {
		setEntry(&dict->trueEntry, &key, &value);
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == false) {
		setEntry(&dict->falseEntry, &key, &value);
	} else

	if (IS_NUMBER(key)) {
		if (entryArraySet(&dict->numberEntries, &dict->numberCapacity, dict->numberCount, key, value, ((int)AS_NUMBER(key)) % dict->numberCapacity)) {
			dict->numberCount++;
		}
	} else

	if (IS_STRING(key)) {
		if (entryArraySet(&dict->stringEntries, &dict->stringCapacity, dict->stringCount, key, value, hashString(AS_STRING(key), strlen(AS_STRING(key))) % dict->stringCapacity)) {
			dict->stringCount++;
		}
	}

	//TODO: interpolated strings
}

void dictionaryDelete(Dictionary* dict, Literal key) {
	if (IS_NIL(key)) {
		freeEntry(&dict->nilEntry);
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == true) {
		freeEntry(&dict->trueEntry);
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == false) {
		freeEntry(&dict->falseEntry);
	} else

	if (IS_NUMBER(key)) {
		Entry* entry = entryArrayGet(dict->numberEntries, dict->numberCapacity, key, ((int)AS_NUMBER(key)) % dict->numberCapacity);
		freeEntry(entry);
		entry->value = TO_BOOL_LITERAL(true);//tombstone
	} else

	if (IS_STRING(key)) {
		Entry* entry = entryArrayGet(dict->stringEntries, dict->stringCapacity, key, hashString(AS_STRING(key), strlen(AS_STRING(key))) % dict->stringCapacity);
		freeEntry(entry);
		entry->value = TO_BOOL_LITERAL(true);//tombstone
	}
}

//important
void dictionaryCopy(Dictionary* target, Dictionary* source) {
	setEntry(&target->nilEntry, &source->nilEntry.key, &source->nilEntry.value);
	setEntry(&target->trueEntry, &source->trueEntry.key, &source->trueEntry.value);
	setEntry(&target->falseEntry, &source->falseEntry.key, &source->falseEntry.value);

	for (int i = 0; i < source->numberCount; i++) {
		if (!IS_NIL(source->numberEntries[i].key)) {
			dictionarySet(target, source->numberEntries[i].key, source->numberEntries[i].value);
		}
	}

	for (int i = 0; i < source->stringCount; i++) {
		if (!IS_NIL(source->stringEntries[i].key)) {
			dictionarySet(target, source->stringEntries[i].key, source->stringEntries[i].value);
		}
	}
}
