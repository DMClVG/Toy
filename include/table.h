#ifndef CTOY_TABLE_H
#define CTOY_TABLE_H

#include "value.h"
#include "object.h"
#include "common.h"

//TODO: benchmark this
#define TABLE_MAX_LOAD 0.75

typedef struct {
	ObjectString* key; //TODO: more than just strings as hashes
	Value value;
} Entry;

struct sTable {
	int capacity;
	int count; //entries PLUS tombstones
	Entry* entries;
};

void initTable(Table* table);
void freeTable(Table* table);

bool tableGet(Table* table, ObjectString* key, Value* value);
bool tableSet(Table* table, ObjectString* key, Value value);
bool tableDelete(Table* table, ObjectString* key);

//util functions
void tableCopyAll(Table* from, Table* to);
ObjectString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);

#endif
