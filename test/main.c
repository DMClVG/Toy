/* DOCS: This is the entry point of the test suite
*/

//TODO: write tests
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

void green() {
#ifdef PLATFORM_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, 2);
#else
	printf("\x1B[32m");
#endif
}

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
		green(); \
		printf("+ pass %s\n", fname); \
		reset(); \
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
#ifdef PLATFORM_WINDOWS
	sprintf(buffer, "out test/scripts/%s", fname);
#else
	sprintf(buffer, "./out test/scripts/%s", fname);
#endif
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
	TEST("print.toy", "hello world\n");
	TEST("numbers.toy", "1\n3\n-1\n4\n2\n0\ntrue\nfalse\n");
	TEST("strings.toy", "foo\nbar\nfoobar\nbuzz\n");
	TEST("groupings.toy", "0.6\n0.2\n");
	TEST("long_literals.toy", "44850\n");

	//finally
	printf("[Final Result]: %d / %d \n", passes, passes + failures);

	return failures;
}