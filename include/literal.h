#ifndef TOY_LITERAL_H
#define TOY_LITERAL_H

#include "common.h"

#include <string.h>

typedef enum {
	LITERAL_NIL,
	LITERAL_BOOL,
	LITERAL_NUMBER,
	LITERAL_STRING,
	//TODO: interpolated strings
	LITERAL_ARRAY,
	LITERAL_DICTIONARY,
	LITERAL_FUNCTION,
	LITERAL_EVENT,
} LiteralType;

typedef struct {
	LiteralType type;
	//NOTE: add an extra field here if "number" is a double
	union {
		bool boolean;
		float number;
		struct {
			char* ptr;
			int length; //could possibly cut it down further by removing this
		} string;

		//experimental
		void* array;
		void* dictionary;
		void* function;
		void* event;
	} as;
} Literal;

#define IS_NIL(value)				((value).type == LITERAL_NIL)
#define IS_BOOL(value)				((value).type == LITERAL_BOOL)
#define IS_NUMBER(value)			((value).type == LITERAL_NUMBER)
#define IS_STRING(value)			((value).type == LITERAL_STRING)
#define IS_ARRAY(value)				((value).type == LITERAL_ARRAY)
#define IS_DICTIONARY(value)		((value).type == LITERAL_DICTIONARY)
#define IS_FUNCTION(value)			((value).type == LITERAL_FUNCTION)
#define IS_EVENT(value)				((value).type == LITERAL_EVENT)

#define AS_BOOL(value)				((value).as.boolean)
#define AS_NUMBER(value)			((value).as.number)
#define AS_STRING(value)			((value).as.string.ptr)
#define AS_ARRAY_PTR(value)
#define AS_DICTIONARY_PTR(value)
#define AS_FUNCTION_PTR(value)		((Function*)((value).as.function))
#define AS_EVENT_PTR(value)

#define TO_NIL_LITERAL				((Literal){LITERAL_NIL,			{ .number = 0 }})
#define TO_BOOL_LITERAL(value)		((Literal){LITERAL_BOOL,		{ .boolean = value }})
#define TO_NUMBER_LITERAL(value)	((Literal){LITERAL_NUMBER,		{ .number = value }})
#define TO_STRING_LITERAL(value)	_toStringLiteral(value)
#define TO_ARRAY_PTR
#define TO_DICTIONARY_PTR
#define TO_FUNCTION_PTR(value)		((Literal){LITERAL_FUNCTION,	{ .function = (Function*)value }})
#define TO_EVENT_PTR

typedef struct {
	int capacity;
	int count;
	Literal* literals;
} LiteralArray;

void initLiteralArray(LiteralArray* array);
void writeLiteralArray(LiteralArray* array, Literal value);
void freeLiteralArray(LiteralArray* array);

void printLiteral(Literal literal);
int findLiteral(LiteralArray* array, Literal literal);
void freeLiteral(Literal literal);

#define IS_TRUTHY(x) _isTruthy(x)

#define STRLEN(lit) ((lit).as.string.length)

//BUGFIX: macros are not functions
bool _isTruthy(Literal x);
Literal _toStringLiteral(char* cstr);

#endif