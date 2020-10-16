#ifndef TOY_DICTIONARY_H
#define TOY_DICTIONARY_H

#include "common.h"
#include "literal.h"

//TODO: benchmark this
#define DICTIONARY_MAX_LOAD 0.75

typedef struct {
	Literal key;
	Literal value;
} Entry;

typedef struct {
	//table of string keys
	int capacity;
	int count;
	Entry* entries;
} Dictionary;

void initDictionary(Dictionary* dict);
void freeDictionary(Dictionary* dict);

Literal dictionaryGet(Dictionary* dict, Literal key);
void dictionarySet(Dictionary* dict, Literal key, Literal value);
void dictionaryDelete(Dictionary* dict, Literal key);

bool dictionaryDeclared(Dictionary* dict, Literal key);
void dictionaryCopy(Dictionary* target, Dictionary* source);

#endif