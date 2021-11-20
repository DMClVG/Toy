#ifndef TOY_VALUE_H_
#define TOY_VALUE_H_

#include "common.h"

//forward declare object stuff
typedef struct sObject Object;
typedef struct sStringObject StringObject;
typedef struct sArrayObject ArrayObject;
typedef struct sDictionaryObject DictionaryObject;
typedef struct sTable Table;

typedef enum ValueType {
	VALUE_NULL,
	VALUE_BOOL,
	VALUE_INTEGER,
	VALUE_FLOAT,
	VALUE_OBJECT, //for heap allocated stuff
} ValueType;

typedef struct sValue {
	ValueType type;
	union {
		bool boolean;
		int integer;
		float floating;
		Object* object;
	} as;
} Value;

#define IS_NULL(value)    ((value).type == VALUE_NULL)
#define IS_BOOL(value)    ((value).type == VALUE_BOOL)
#define IS_INTEGER(value) ((value).type == VALUE_INTEGER)
#define IS_FLOAT(value)   ((value).type == VALUE_FLOAT)
#define IS_OBJECT(value)  ((value).type == VALUE_OBJECT)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_INTEGER(value) ((value).as.integer)
#define AS_FLOAT(value)   ((value).as.floating)
#define AS_OBJECT(value)  ((value).as.object)

#define TO_NULL_VALUE()         ((Value){VALUE_NULL,    { .object = NULL }})
#define TO_BOOL_VALUE(value)    ((Value){VALUE_BOOL,    { .boolean = value }})
#define TO_INTEGER_VALUE(value) ((Value){VALUE_INTEGER, { .integer = value }})
#define TO_FLOAT_VALUE(value)   ((Value){VALUE_FLOAT,   { .floating = value }})
#define TO_OBJECT_VALUE(value)  ((Value){VALUE_OBJECT,  { .object = (Object*)value }})

typedef struct ValueArray {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

bool valuesEqual(Value a, Value b);

void printValue(Value value);

#endif //TOY_VALUE_H_
