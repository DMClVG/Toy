#ifndef TOY_TABLE_H_
#define TOY_TABLE_H_

#include "common.h"
#include "object.h"
#include "value.h"

//TODO: benchmark this
#define TABLE_MAX_LOAD 0.75

typedef struct Entry {
	Object* key;
	Value value;
} Entry;

struct sTable {
	int capacity;
	int count; //entries PLUS tombstones
	Entry* entries;
};

void initTable(Table* table);
void freeTable(Table* table);

bool tableGet(Table* table, Object* key, Value* value);
bool tableSet(Table* table, Object* key, Value value);
bool tableDelete(Table* table, Object* key);

//util functions
void tableCopyAll(Table* from, Table* to);

#endif