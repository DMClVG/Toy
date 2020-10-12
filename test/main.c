/* DOCS: This is the entry point of the test suite
*/

//TODO: write tests
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

void red() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 12);
#else
	printf("\x1B[31m");
#endif
}

void reset() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 7);
#else
	printf("\x1B[0m");
#endif
}

#define TEST(fname, expected) \
	if (!runTestFile(fname, expected)) { \
		printf("+ pass %s\n", fname); \
		passes++; \
	} else { \
		red(); \
		printf("- fail %s\n", fname); \
		reset(); \
		failures++; \
	}

int runTestFile(const char* fname, const char* expected) {
	char buffer[1024];
	memset(buffer, 0, 1024);

	//open the file
	sprintf(buffer, "out test/scripts/%s", fname);
	FILE* handle = popen(buffer, "r");

	//read in the file toy output
	memset(buffer, 0, 1024);
	fread(buffer, 1, 1024, handle);

	//close the stream
	fclose(handle);

	//compare against the expected result
	return strcmp(buffer, expected);
}

int main(int argc, const char* argv[]) {
	//initialize these
	int passes = 0, failures = 0;

	//run each test
	TEST("1.toy", "hello world\n");

	//finally
	printf("[Fineal Result]: %d / %d \n", passes, passes + failures);

	return failures;
}