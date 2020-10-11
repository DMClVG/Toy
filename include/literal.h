#ifndef TOY_LITERAL_H
#define TOY_LITERAL_H

#include "common.h"

typedef enum {
	LITERAL_NIL,
	LITERAL_BOOL,
	LITERAL_NUMBER,
	LITERAL_STRING,
	//TODO: interpolated strings
} LiteralType;

typedef struct {
	LiteralType type;
	union {
		bool boolean;
		double number;
		char* string;
	} as;
} Literal;

#define IS_NIL(value)    ((value).type == LITERAL_NIL)
#define IS_BOOL(value)   ((value).type == LITERAL_BOOL)
#define IS_NUMBER(value) ((value).type == LITERAL_NUMBER)
#define IS_STRING(value) ((value).type == LITERAL_STRING)

#define AS_BOOL(value)   ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_STRING(value) ((value).as.string)

#define TO_NIL_LITERAL           ((Literal){LITERAL_NIL,		{ .number = 0 }})
#define TO_BOOL_LITERAL(value)   ((Literal){LITERAL_BOOL,		{ .boolean = value }})
#define TO_NUMBER_LITERAL(value) ((Literal){LITERAL_NUMBER,		{ .number = value }})
#define TO_STRING_LITERAL(value) ((Literal){LITERAL_STRING,		{ .string = (char*)value }})

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

char* copyAndParseString(char* original, int originalLength);

#endif
