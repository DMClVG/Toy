#include "table.h"
#include "memory.h"
#include "object.h"

#include <stdlib.h>
#include <string.h>

void initTable(Table* table) {
	table->capacity = 0;
	table->count = 0;
	table->entries = NULL;
}

void freeTable(Table* table) {
	FREE_ARRAY(Entry, table->entries, table->capacity);
	initTable(table);
}

static Entry* findEntry(Entry* entries, int capacity, Object* key) {
	uint32_t index = key->hash % capacity;
	Entry* tombstone = NULL;

	//linear probing and collision hashing
	for (;;) {
		Entry* entry = &entries[index];

		if (entry->key == NULL) {
			//TODO: refactor this
			if (IS_NULL(entry->value)) {
				//found a truly empty bucket
				return tombstone != NULL ? tombstone : entry;
			} else {
				//we found a tombstone
				if (tombstone == NULL) {
					tombstone = entry;
				}
			}
		} else if (objectsEqual(key, entry->key)) {
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
		entries[i].value = TO_NULL_VALUE();
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

bool tableGet(Table* table, Object* key, Value* value) {
	if (table->count == 0) return false;

	Entry* entry = findEntry(table->entries, table->capacity, key);

	if (entry->key == NULL) {
		return false;
	}

	*value = entry->value;
	return true;
}

bool tableSet(Table* table, Object* key, Value value) {
	//expand table
	if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
		int capacity = GROW_CAPACITY(table->capacity);
		adjustCapacity(table, capacity);
	}

	Entry* entry = findEntry(table->entries, table->capacity, key);

	bool isNewKey = entry->key == NULL;
	if (isNewKey && IS_NULL(entry->value)) {
		table->count++;
	}

	entry->key = key;
	entry->value = value;

	return isNewKey;
}

bool tableDelete(Table* table, Object* key) {
	if (table->count == 0) return false;

	Entry* entry = findEntry(table->entries, table->capacity, key);

	if (entry->key == NULL) {
		return false;
	}

	//tombstone
	entry->key = NULL;
	entry->value = TO_BOOL_VALUE(true);

	return true;
}

void tableCopyAll(Table* from, Table* to) {
	for(int i = 0; i < from->capacity; i++) {
		Entry* entry = &from->entries[i];
		if (entry->key != NULL) {
			tableSet(to, entry->key, entry->value);
		}
	}
}
