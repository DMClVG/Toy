#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

int memoryAllocated = 0;

static void color() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 1);
#endif
}

static void reset() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 7);
#endif
}

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
	color();
	if (newSize == 0) {
		if (oldSize > 0) {
			printf("freeing %d units of memory\n", oldSize);
		}
		free(pointer);

		memoryAllocated -= oldSize;
		printf("memoryAllocated: %d\n", memoryAllocated);
		reset();
		return NULL;
	}

	printf("Allocating %d units of memory (ptr: %d)\n", newSize, pointer);
	void* mem = realloc(pointer, newSize);

	if (mem == NULL) {
		fprintf(stderr, "[Internal]Memory allocation error\n");
		exit(1);
	}

	memoryAllocated -= oldSize;
	memoryAllocated += newSize;
	printf("memoryAllocated: %d\n", memoryAllocated);\
	reset();
	return mem;
}