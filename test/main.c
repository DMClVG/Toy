/* DOCS: This is the entry point of the test suite
*/

//TODO: write tests
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#include "dictionary_test.h"
#include "scope_test.h"

#include "../source/chunk.c"
#include "../source/dictionary.c"
#include "../source/function.c"
#include "../source/literal.c"
#include "../source/memory.c"
#include "../source/scope.c"

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

int runTestFile(const char* fname, const char* expected) {
	char buffer[1024];
	memset(buffer, 0, 1024);

	//open the file
#ifdef PLATFORM_WINDOWS
	sprintf(buffer, "Toy -f test/scripts/%s", fname);
#else
	sprintf(buffer, "./Toy -f test/scripts/%s", fname);
#endif
	FILE* handle = popen(buffer, "r");

	if (!handle) {
		fprintf(stderr, "Failed to open FILE stream: %s\n", buffer);
		return 1;
	}

	//read in the file toy output
	memset(buffer, 0, 1024);
	size_t resultLength = fread(buffer, 1, 1024, handle);

	//close the stream
	fclose(handle);

	if (resultLength != strlen(expected)) {
		printf("Failed to read the whole expected output (only read %d bytes)\n", (int)resultLength);
	}

	//compare against the expected result
	return strcmp(buffer, expected);
}

#define TEST_FILE(fname, expected) \
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

#define TEST_FUNCTION(fn) \
	{ \
		int s = 0, f = 0; \
		fn(&s, &f); \
		if (f > 0) { \
			red(); \
			if (s > 0) { \
				printf("+ part %s %d / %d\n", #fn, s, s + f); \
			} else { \
				printf("- fail %s %d / %d\n", #fn, s, s + f); \
			} \
		} else { \
			green(); \
			printf("+ pass %s %d / %d\n", #fn, s, s + f); \
		} \
		reset(); \
		passes += s; \
		failures += f; \
	}

int main(/* int argc, const char* argv[] */) {
	const int testCount = 40;

	//initialize these
	int passes = 0, failures = 0;

	//run each test
	TEST_FILE("print.toy", "hello world\n");
	TEST_FILE("numbers.toy", "1\n3\n-1\n4\n2\n0\ntrue\nfalse\n");
	TEST_FILE("strings.toy", "foo\nbar\nfoobar\nbuzz\n");
	TEST_FILE("groupings.toy", "0.6\n0.2\n");
	TEST_FILE("long_literals.toy", "44850\n");
	TEST_FILE("variables.toy", "hello world\nhello world\nhello world goodnight world\nfoobar\n");
	TEST_FILE("scope.toy", "1\n2\n3\n1\n1\n1\n");
	TEST_FILE("compound_assignment.toy", "2\n4\n1\n-9\n3\n");
	TEST_FILE("functions_and_returns.toy", "inside f\nargument\n342\n1\n2\n3\n");

	TEST_FUNCTION(dictionary_test);
	TEST_FUNCTION(scope_test);

	//finally
	if (passes + failures != testCount) {
		red();
	}
	printf("[Final Result]: %d / %d (expected %d tests)\n", passes, passes + failures, testCount);
	reset();

	return failures;
}