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
	//TODO: arrays (LiteralArrays)
	//TODO: dictionaries (Tables)
} LiteralType;

typedef struct {
	LiteralType type;
	uint32_t __unused; //4 bytes reserved for future use
	union {
		bool boolean;
		double number;
		struct {
			char* ptr;
			int length;
		} string;
	} as;
} Literal;

#define IS_NIL(value)		((value).type == LITERAL_NIL)
#define IS_BOOL(value)		((value).type == LITERAL_BOOL)
#define IS_NUMBER(value)	((value).type == LITERAL_NUMBER)
#define IS_STRING(value)	((value).type == LITERAL_STRING)

#define AS_BOOL(value)		((value).as.boolean)
#define AS_NUMBER(value)	((value).as.number)
#define AS_STRING(value)	((value).as.string.ptr)

#define TO_NIL_LITERAL				((Literal){LITERAL_NIL,		0, { .number = 0 }})
#define TO_BOOL_LITERAL(value)		((Literal){LITERAL_BOOL,	0, { .boolean = value }})
#define TO_NUMBER_LITERAL(value)	((Literal){LITERAL_NUMBER,	0, { .number = value }})
#define TO_STRING_LITERAL(value)	((Literal){LITERAL_STRING,	0, { .string.ptr = (char*)value, .string.length = strlen((char*)value) }})

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

#define IS_TRUTHY(x) (IS_NIL(x) || (IS_BOOL(x) && AS_BOOL(x)) || (IS_NUMBER(x) && AS_NUMBER(x) != 0))

#define STRLEN(lit) ((lit).as.string.length)

#endif