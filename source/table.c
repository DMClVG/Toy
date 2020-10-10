#include "table.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

//utility functions
static uint32_t hashString(const char* string, int length) {
	uint32_t hash = 2166136261u;

	for (int i = 0; i < length; i++) {
		hash *= string[i];
		hash *= 16777619;
	}

	return hash;
}

//init functions
void initTable(Table* table) {
	table->capacity = 0;
	table->count = 0;
	table->entries = NULL;
}

void freeTable(Table* table) {
	FREE_ARRAY(Entry, table->entries, table->capacity);
	initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity, char* key) {
	uint32_t index = hashString(key, strlen(key)) % capacity;
	Entry* tombstone = NULL;

	//linear probing and collision hashing
	for (;;) {
		Entry* entry = &entries[index];

		if (entry->key == NULL) {
			//TODO: refactor this
			if (IS_NIL(entry->value)) {
				//found a truly empty bucket
				return tombstone != NULL ? tombstone : entry;
			} else {
				//we found a tombstone
				if (tombstone == NULL) {
					tombstone = entry;
				}
			}
		} else if (memcmp(entry->key, key, strlen(key)) == 0) { //a bit slow
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static void adjustCapacity(Table* table, int capacity) {
	Entry* entries = ALLOCATE(Entry, capacity);

	//zero the new array
	for (int i = 0; i < capacity; i++) {
		entries[i].key = NULL;
		entries[i].value = TO_NIL_LITERAL;
	}

	//move the array
	table->count = 0;

	for (int i = 0; i < table->capacity; i++) {
		//grab each existing bucket
		Entry* entry = &table->entries[i];
		if (entry->key == NULL) continue;

		Entry* dest = findEntry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
		table->count++;
	}

	//assign
	FREE_ARRAY(Entry, table->entries, table->capacity);
	table->entries = entries;
	table->capacity = capacity;
}

bool tableGet(Table* table, char* key, Literal* value) {
	if (table->count == 0) return false;

	Entry* entry = findEntry(table->entries, table->capacity, key);

	if (entry->key == NULL) {
		return false;
	}

	*value = entry->value;
	return true;
}

bool tableSet(Table* table, char* key, Literal value) {
	//expand table
	if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
		int capacity = GROW_CAPACITY(table->capacity);
		adjustCapacity(table, capacity); //custom instead of GROW_ARRAY
	}

	Entry* entry = findEntry(table->entries, table->capacity, key);

	bool isNewKey = entry->key == NULL;
	if (isNewKey && IS_NIL(entry->value)) {
		table->count++;
	}

	entry->key = key;
	entry->value = value;

	return isNewKey;
}

bool tableDelete(Table* table, char* key) {
	if (table->count == 0) return false;

	Entry* entry = findEntry(table->entries, table->capacity, key);

	if (entry->key == NULL) {
		return false;
	}

	//tombstone
	entry->key = NULL;
	entry->value = TO_BOOL_LITERAL(true);

	return true;
}

void tableCopyAll(Table* original, Table* target) {
	for(int i = 0; i < original->capacity; i++) {
		Entry* entry = &original->entries[i];
		if (entry->key != NULL) {
			tableSet(target, entry->key, entry->value);
		}
	}
}
