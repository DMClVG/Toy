#ifndef TOY_MEMORY_H
#define TOY_MEMORY_H

#include "common.h"

#define ALLOCATE(type, count) ((type*)reallocate(NULL, 0, sizeof(type) * (count)))
#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)
#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)
#define GROW_ARRAY(type, pointer, oldCount, count) (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (count))
#define FREE_ARRAY(type, pointer, oldCount) reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif
//TODO: GROW_CAPACITY_FAST - this is an optimisation for the tables, grow a table twice as fast (benchmark it)