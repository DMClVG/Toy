#ifndef TOY_OBJECT_H_
#define TOY_OBJECT_H_

#include "value.h"
#include "table.h"
#include "common.h"

//object types
typedef enum { //TODO: mark as EMPTY for GC
	OBJECT_STRING,
	OBJECT_ARRAY,
	OBJECT_DICTIONARY,
} ObjectType;

//base object
typedef struct sObject {
	ObjectType type;
	uint32_t hash;
	Object* next; //for the object pool (memory management)
} Object;

//string object
typedef struct sStringObject {
	Object object;
	int length;
	char* chars;
} StringObject;

//array object
typedef struct sArrayObject {
	Object object;
	ValueArray* array; //gets stored in a table
} ArrayObject;

//dictionary object
typedef struct sDictionaryObject {
	Object object;
	Table* table; //gets stored in a table
} DictionaryObject;

#define OBJECT_TYPE(obj) ((obj)->type)

#define IS_STRING_OBJECT(value)     isObjectType(value, OBJECT_STRING)
#define IS_ARRAY_OBJECT(value)      isObjectType(value, OBJECT_ARRAY)
#define IS_DICTIONARY_OBJECT(value) isObjectType(value, OBJECT_DICTIONARY)

#define AS_STRING(value)     ((StringObject*)AS_OBJECT(value))
#define AS_CSTRING(value)    (((StringObject*)AS_OBJECT(value))->chars)
#define AS_ARRAY(value)      ((ArrayObject*)AS_OBJECT(value))
#define AS_DICTIONARY(value) ((DictionaryObject*)AS_OBJECT(value))

StringObject* takeString(Object** pool, Table* stringTable, char* chars, int length);
StringObject* copyString(Object** pool, Table* stringTable, const char* chars, int length);
ArrayObject* takeArray(Object** pool, Table* arrayTable, ValueArray* array);
DictionaryObject* takeDictionary(Object** pool, Table* dictionaryTable, Table* dictTable);

bool objectsEqual(Object* a, Object* b);

//utils
void freeObjectPool(Object** pool);
Object* mergeObjectPools(Object* a, Object* b);

void printObject(Object* object); //debugging

static inline bool isObjectType(Value value, ObjectType type) {
	return IS_OBJECT(value) && AS_OBJECT(value)->type == type;
}

#endif //TOY_OBJECT_H_
