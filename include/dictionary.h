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
	//a key of "true" means these are present
	Entry nilEntry;
	Entry trueEntry;
	Entry falseEntry;

	//table of number keys
	int numberCapacity;
	int numberCount;
	Entry* numberEntries;

	//table of string keys
	int stringCapacity;
	int stringCount;
	Entry* stringEntries;
} Dictionary;

void initDictionary(Dictionary* dict);
void freeDictionary(Dictionary* dict);

Literal* dictionaryGet(Dictionary* dict, Literal key);
void dictionarySet(Dictionary* dict, Literal key, Literal value);
void dictionaryDelete(Dictionary* dict, Literal key);

void dictionaryCopy(Dictionary* target, Dictionary* source);

#endif