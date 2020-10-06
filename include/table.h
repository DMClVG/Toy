#ifndef CTOY_TABLE_H
#define CTOY_TABLE_H

#include "literal.h"
#include "common.h"

//TODO: benchmark this
#define TABLE_MAX_LOAD 0.75

typedef struct {
	char* key;
	Literal value;
} Entry;

typedef struct {
	int capacity;
	int count; //entries PLUS tombstones
	Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);

bool tableGet(Table* table, char* key, Literal* value);
bool tableSet(Table* table, char* key, Literal value);
bool tableDelete(Table* table, char* key);

//util functions
void tableCopyAll(Table* original, Table* target);

#endif
