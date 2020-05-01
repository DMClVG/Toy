#ifndef CTOY_VALUE_H
#define CTOY_VALUE_H

#include "common.h"

typedef enum {
	VAL_NIL,
	VAL_BOOL,
	VAL_NUMBER,
	VAL_OBJECT, //for heap allocated stuff
} ValueType;

//forward declare object stuff
typedef struct sObject Object;
typedef struct sObjectString ObjectString;
typedef struct sTable Table;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
		Object* object;
	} as;
} Value;

#define IS_NIL(value)    ((value).type == VAL_NIL)
#define IS_BOOL(value)   ((value).type == VAL_BOOL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJECT(value) ((value).type == VAL_OBJECT)

#define AS_BOOL(value)   ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJECT(value) ((value).as.object)

#define NIL_VAL           ((Value){VAL_NIL,    { .number = 0 }})
#define BOOL_VAL(value)   ((Value){VAL_BOOL,   { .boolean = value }})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, { .number = value }})
#define OBJECT_VAL(value) ((Value){VAL_OBJECT, { .object = (Object*)value }})

typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

bool valuesEqual(Value a, Value b);

void printValue(Value value);

#endif
