#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

int memoryAllocated = 0;

static void blue() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 3);
#endif
}

static void purple() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 5);
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
	if (newSize == 0) {
		if (oldSize > 0) {
			purple();
			printf("freeing %d units of memory\n", oldSize);
			reset();
		}
		free(pointer);

		memoryAllocated -= oldSize;
		printf("memoryAllocated: %d\n", memoryAllocated);
		return NULL;
	}

	blue();
	printf("Allocating %d units of memory (oldSize: %d, ptr: %d)\n", newSize, oldSize, pointer);
	reset();
	void* mem = realloc(pointer, newSize);

	if (mem == NULL) {
		fprintf(stderr, "[Internal]Memory allocation error\n");
		exit(1);
	}

	memoryAllocated -= oldSize;
	memoryAllocated += newSize;
	printf("memoryAllocated: %d\n", memoryAllocated);

#ifdef PLATFORM_WINDOWS
	if (newSize - oldSize != _msize(mem)) {
		printf("real: %d, fake: %d\n", _msize(mem), newSize - oldSize);
	}
#endif
	return mem;
}