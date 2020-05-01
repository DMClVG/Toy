#ifndef CTOY_OBJECT_H
#define CTOY_OBJECT_H

#include "value.h"
#include "table.h"
#include "common.h"

typedef enum {
	OBJ_STRING,
} ObjectType;

struct sObject {
	ObjectType type;
	Object* next; //for the object pool
};

struct sObjectString {
	Object object;
	int length;
	char* chars;
	uint32_t hash;
};

#define OBJECT_TYPE(obj) ((obj)->type)

#define IS_STRING(value) isObjectType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjectString*)AS_OBJECT(value))
#define AS_CSTRING(value) (((ObjectString*)AS_OBJECT(value))->chars)

ObjectString* takeString(Object** pool, Table* stringTable, char* chars, int length);
ObjectString* copyString(Object** pool, Table* stringTable, const char* chars, int length);
void freeObjectPool(Object** pool);

void printObject(Object* object);

static inline bool isObjectType(Value value, ObjectType type) {
	return IS_OBJECT(value) && AS_OBJECT(value)->type == type;
}

#endif