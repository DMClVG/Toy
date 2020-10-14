#include "dictionary_test.h"

#include "../source/memory.c"
#include "../source/literal.c"
#include "../source/dictionary.c"

#define SUCCESS (*successes)++
#define FAILURE (*failures)++
#define TEST(x) if (x) SUCCESS; else FAILURE

#ifdef LN
#undef LN
#endif

#include <stdio.h>
//#define LN printf("%s: %d\n", __FILE__, __LINE__);
#define LN

void dictionary_test(int* successes, int* failures) {
/*
	//init & free an empty dict
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);
LN		freeDictionary(&dictionary);
LN		SUCCESS;
LN	}

	//store a single number with a string key
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);
LN
LN		Literal key = TO_STRING_LITERAL("hello world");
LN		Literal value = TO_NUMBER_LITERAL(42);
LN
LN		dictionarySet(&dictionary, key, value);
LN		Literal result = *dictionaryGet(&dictionary, key);
LN
LN		TEST(AS_NUMBER(result) == 42);
LN		freeDictionary(&dictionary);
LN	}

	//store a single number with a number key
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);
LN
LN		Literal key = TO_NUMBER_LITERAL(21795);
LN		Literal value = TO_NUMBER_LITERAL(21795);
LN
LN		dictionarySet(&dictionary, key, value);
LN		Literal result = *dictionaryGet(&dictionary, key);
LN
LN		TEST(AS_NUMBER(result) == 21795);
LN		freeDictionary(&dictionary);
LN	}

	//store a single number with a true key
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);
LN
LN		Literal key = TO_BOOL_LITERAL(true);
LN		Literal value = TO_NUMBER_LITERAL(231268);
LN
LN		dictionarySet(&dictionary, key, value);
LN		Literal result = *dictionaryGet(&dictionary, key);
LN
LN		TEST(AS_NUMBER(result) == 231268);
LN		freeDictionary(&dictionary);
LN	}

	//store a single number with a false key
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);
LN
LN		Literal key = TO_BOOL_LITERAL(false);
LN		Literal value = TO_NUMBER_LITERAL(8891);
LN
LN		dictionarySet(&dictionary, key, value);
LN		Literal result = *dictionaryGet(&dictionary, key);
LN
LN		TEST(AS_NUMBER(result) == 8891);
LN		freeDictionary(&dictionary);
LN	}

	//store a single number with a null key
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);
LN
LN		Literal key = TO_NIL_LITERAL;
LN		Literal value = TO_NUMBER_LITERAL(420);

LN		dictionarySet(&dictionary, key, value);
LN		Literal result = *dictionaryGet(&dictionary, key);
LN
LN		TEST(AS_NUMBER(result) == 420);
LN		freeDictionary(&dictionary);
LN	}
*/
	//store a large number of values in a dictionary
LN	{
LN		Dictionary dictionary;
LN		initDictionary(&dictionary);

LN		for (int i = 0; i < 500; i++) {
LN			Literal key = TO_NUMBER_LITERAL(i);
//			printf("size, count, capacity: %d / %d / %d\n", i, dictionary.numberCount, dictionary.numberCapacity);
LN			dictionarySet(&dictionary, key, TO_STRING_LITERAL("Hello world"));
LN		}

LN		//NOTE: specific number; will change as DICTIONARY_MAX_LOAD changes
LN		TEST(dictionary.numberCapacity == 1024);
LN		TEST(dictionary.numberCount == 500);
LN		freeDictionary(&dictionary);
LN	}

	//test table copying
	{
LN		Dictionary source;
LN		initDictionary(&source);

LN		for (int i = 0; i < 500; i++) {
LN			dictionarySet(&source, TO_NUMBER_LITERAL(i), TO_STRING_LITERAL("Hello world"));
LN		}

LN		Dictionary dest;
LN		initDictionary(&dest);

LN		dictionaryCopy(&dest, &source);

		//NOTE: specific number; will change as DICTIONARY_MAX_LOAD changes
LN		TEST(dest.numberCapacity == 1024);
LN		TEST(dest.numberCount == 500);

LN		freeDictionary(&source);
LN		freeDictionary(&dest);
LN	}

	//test string ownership
LN	{
LN		Dictionary dictionary;

LN		initDictionary(&dictionary);

LN		Literal key = TO_STRING_LITERAL("key");
LN		Literal val = TO_STRING_LITERAL("value");

LN		dictionarySet(&dictionary, key, val);

LN		*AS_STRING(key) = 'a';
LN		*AS_STRING(val) = 'b';

LN		Literal result = *dictionaryGet(&dictionary, TO_STRING_LITERAL("key"));

LN		TEST(strcmp(AS_STRING(result), "value") == 0);

LN		freeDictionary(&dictionary);
LN	}

	//TODO: deletion test
}