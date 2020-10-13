#include "dictionary.h"
#include "memory.h"

#include <stdio.h> //debugging
//#define LN printf("%s: %d\n", __FILE__, __LINE__);
#define LN

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
	for (;;) {
printf("loop: (%d / %d)\n", index, capacity);
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
LN	printf("adjusting capacity: %d -> %d\n", oldCapacity, capacity);

	//new entries
LN	Entry* newEntries = ALLOCATE(Entry, capacity); //ERROR: this line fails

	printf("old, new, entries: %d %d %d\n", oldCapacity, capacity, newEntries);

	if (newEntries == NULL) {
		printf("Well, we're boned.\n");
	}

LN	for (int i = 0; i < capacity; i++) {
LN		newEntries[i].key = TO_NIL_LITERAL;
LN		newEntries[i].value = TO_NIL_LITERAL;
LN	}

	//move the old array into the new one
LN	for (int i = 0; i < oldCapacity; i++) {
LN		if (IS_NIL(array[i].key)) {
LN			continue;
LN		}

		//determine where to place the new entry
LN		int index = -1;

LN		if (IS_NUMBER(array[i].key)) {
LN			index = ((int)AS_NUMBER(array[i].key)) % capacity;
LN		}

LN		if (IS_STRING(array[i].key)) {
LN			index = hashString(AS_STRING(array[i].key), strlen(AS_STRING(array[i].key))) % capacity;
LN		}

LN		if (index == -1) { //WARNING: This is definitely the problem
	printLiteral(array[i].key);
	printf("%d -> %d, %d\n", oldCapacity, capacity, i);
	printf("array location: %d\n", array);
LN			while(true);
LN		}

		//place the key and value in the new array (reusing string memory)
LN		printf("%d\n", array[i].key.type);
LN		printf("%d %d %d %d\n", capacity, oldCapacity, index, i);
LN		newEntries[index].key = array[i].key;
LN		newEntries[index].value = array[i].value;
LN	}

	//clear the old array
LN	FREE_ARRAY(Entry, array, oldCapacity);
LN	return newEntries;
}

bool entryArraySet(Entry** array, int* capacityPtr, int count, Literal key, Literal value, int startPos) {
	//expand array
LN	if (count + 1 > *capacityPtr * DICTIONARY_MAX_LOAD) {
LN		int oldCapacity = *capacityPtr;
LN		*capacityPtr = GROW_CAPACITY(*capacityPtr);
LN		*array = adjustCapacity(*array, oldCapacity, *capacityPtr); //custom rather than automatic reallocation

		//recalc start pos
LN		if (IS_NUMBER(key)) {
LN			startPos = ((int)AS_NUMBER(key)) % *capacityPtr;
LN		}
LN		if (IS_STRING(key)) {
LN			startPos = hashString(AS_STRING(key), strlen(AS_STRING(key))) % *capacityPtr;
LN		}
LN	}

LN	Entry* entry = entryArrayGet(*array, *capacityPtr, key, startPos);

	//TODO: count increase
LN	if (IS_NIL(entry->key)) {
LN		setEntry(entry, &key, &value);
LN		return true;
LN	} else {
LN		setEntry(entry, &key, &value);
LN		return false;
LN	}
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

LN	dict->numberCapacity = GROW_CAPACITY(0);
LN	dict->numberCount = 0;
LN	dict->numberEntries = adjustCapacity(NULL, 0, dict->numberCapacity);

LN	dict->stringCapacity = GROW_CAPACITY(0);
LN	dict->stringCount = 0;
LN	dict->stringEntries = adjustCapacity(NULL, 0, dict->stringCapacity);
LN
//	printf("number location: %d\n", dict->numberEntries);
//	printf("string location: %d\n", dict->stringEntries);

//	printLiteral(dict->numberEntries[0].key);
//s	printLiteral(dict->numberEntries[0].value);
}

void freeDictionary(Dictionary* dict) {
	freeEntry(&dict->nilEntry);
	freeEntry(&dict->trueEntry);
	freeEntry(&dict->falseEntry);

	freeEntryArray(dict->numberEntries, dict->numberCapacity);
	freeEntryArray(dict->stringEntries, dict->stringCapacity);
}

//accessors & mutators
Literal* dictionaryGet(Dictionary* dict, Literal key) {
	if (IS_NIL(key)) {
		return &dict->nilEntry.value;
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == true) {
		return &dict->trueEntry.value;
	} else

	if (IS_BOOL(key) && AS_BOOL(key) == false) {
		return &dict->falseEntry.value;
	} else

	if (IS_NUMBER(key)) {
		return &entryArrayGet(dict->numberEntries, dict->numberCapacity, key, ((int)AS_NUMBER(key)) % dict->numberCapacity)->value;
	} else

	if (IS_STRING(key)) {
		return &entryArrayGet(dict->stringEntries, dict->stringCapacity, key, hashString(AS_STRING(key), strlen(AS_STRING(key))) % dict->stringCapacity)->value;
	} else

	{
		return NULL;
	}

	//TODO: interpolated strings
}

void dictionarySet(Dictionary* dict, Literal key, Literal value) {
LN	if (IS_NIL(key)) {
LN		setEntry(&dict->nilEntry, &key, &value);
LN	} else

	if (IS_BOOL(key) && AS_BOOL(key) == true) {
LN		setEntry(&dict->trueEntry, &key, &value);
LN	} else

	if (IS_BOOL(key) && AS_BOOL(key) == false) {
LN		setEntry(&dict->falseEntry, &key, &value);
LN	} else

	if (IS_NUMBER(key)) {
LN		if (entryArraySet(&dict->numberEntries, &dict->numberCapacity, dict->numberCount, key, value, ((int)AS_NUMBER(key)) % dict->numberCapacity)) {
LN			dict->numberCount++;
LN		}
LN	} else

	if (IS_STRING(key)) {
LN		if (entryArraySet(&dict->stringEntries, &dict->stringCapacity, dict->stringCount, key, value, hashString(AS_STRING(key), strlen(AS_STRING(key))) % dict->stringCapacity)) {
LN			dict->stringCount++;
LN		}
LN	}
LN
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
LN	freeDictionary(target);

LN	setEntry(&target->nilEntry, &source->nilEntry.key, &source->nilEntry.value);
LN	setEntry(&target->trueEntry, &source->trueEntry.key, &source->trueEntry.value);
LN	setEntry(&target->falseEntry, &source->falseEntry.key, &source->falseEntry.value);

LN	freeEntryArray(target->numberEntries, target->numberCapacity);
LN	freeEntryArray(target->stringEntries, target->stringCapacity);

LN	for (int i = 0; i < source->numberCount; i++) {
LN		if (!IS_NIL(source->numberEntries[i].key)) {
LN			dictionarySet(target, source->numberEntries[i].key, source->numberEntries[i].value);
LN		}
LN	}

LN	for (int i = 0; i < source->stringCount; i++) {
LN		if (!IS_NIL(source->stringEntries[i].key)) {
LN			dictionarySet(target, source->stringEntries[i].key, source->stringEntries[i].value);
LN		}
LN	}
LN
}

#undef LN