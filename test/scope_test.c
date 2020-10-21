#include "scope_test.h"

#define SUCCESS (*successes)++
#define FAILURE (*failures)++
#define TEST(x) if (x) SUCCESS; else { FAILURE; printf("  Failed test on line %d: %s\n", __LINE__, #x); }

#ifdef LN
#undef LN
#endif

#include <stdio.h>
//#define LN printf("%s: %d\n", __FILE__, __LINE__);
#define LN

void scope_test(int* successes, int* failures) {
	//init & free an empty scope
LN	{
LN		Scope* scope = pushScope(NULL);
LN		popScope(scope);
LN		SUCCESS;
LN	}

	//push & pop scopes
LN	{
LN		Scope* scope = pushScope(NULL);
LN		scope = pushScope(scope);
LN		scope = pushScope(scope);
LN		scope = pushScope(scope);

LN		TEST(scope->ancestor && scope->ancestor->ancestor && scope->ancestor->ancestor->ancestor && scope->ancestor->ancestor->ancestor->ancestor == NULL);

LN		scope = popScope(scope);
LN		scope = popScope(scope);

LN		TEST(scope->ancestor && scope->ancestor->ancestor == NULL);

		scope = popScope(scope); //MUST have an equal number of pushes & pops

LN		popScope(scope);
LN	}

	//declare constants & retreive them
	{
LN		Scope* scope = pushScope(NULL);

LN		bool ret1 = scopeSetConstant(scope, TO_STRING_LITERAL("key"), TO_STRING_LITERAL("value"), true);
LN		bool ret2 = scopeSetConstant(scope, TO_STRING_LITERAL("foo"), TO_STRING_LITERAL("bar"), false); //will fail

		//key-value
LN		bool definedKey = false;
LN		Literal key = scopeGet(scope, TO_STRING_LITERAL("key"), &definedKey);

LN		TEST(definedKey == true && strcmp(AS_STRING(key), "value") == 0);

		//test the declaration check
LN		TEST(ret1 == true && ret2 == false);

LN		popScope(scope);
	}

	//declare variables & retreive them (duplicate of above, but with variables)
	{
LN		Scope* scope = pushScope(NULL);

LN		bool ret1 = scopeSetVariable(scope, TO_STRING_LITERAL("key"), TO_STRING_LITERAL("value"), true);
LN		bool ret2 = scopeSetVariable(scope, TO_STRING_LITERAL("foo"), TO_STRING_LITERAL("bar"), false); //will fail

		//key-value
LN		bool definedKey = false;
LN		Literal key = scopeGet(scope, TO_STRING_LITERAL("key"), &definedKey);

LN		TEST(definedKey == true && strcmp(AS_STRING(key), "value") == 0);

		//test the declaration check
LN		TEST(ret1 == true && ret2 == false);

LN		popScope(scope);
	}

	//test that you can overwrite existing variables, but not constants
	{
LN		Scope* scope = pushScope(NULL);

LN		scopeSetConstant(scope, TO_STRING_LITERAL("key"), TO_STRING_LITERAL("value"), true);
LN		scopeSetVariable(scope, TO_STRING_LITERAL("foo"), TO_STRING_LITERAL("bar"), true);

		//overwrite
LN		int ret1 = scopeSetConstant(scope, TO_STRING_LITERAL("key"), TO_NUMBER_LITERAL(42), false);
LN		int ret2 = scopeSetVariable(scope, TO_STRING_LITERAL("foo"), TO_NUMBER_LITERAL(21795), false);

LN		TEST(ret1 == false && ret2 == true);

LN		TEST(IS_STRING(scopeGet(scope, TO_STRING_LITERAL("key"), NULL)) && strcmp(AS_STRING(scopeGet(scope, TO_STRING_LITERAL("key"), NULL)), "value") == 0);
LN		TEST(IS_NUMBER(scopeGet(scope, TO_STRING_LITERAL("foo"), NULL)) && AS_NUMBER( scopeGet(scope, TO_STRING_LITERAL("foo"), NULL) ) == 21795);

LN		popScope(scope);
	}

	//test scope depth
LN	{
		Scope* scope = pushScope(NULL);

LN		scopeSetConstant(scope, TO_STRING_LITERAL("key"), TO_NUMBER_LITERAL(1), true);
LN		scope = pushScope(scope);
LN		TEST(IS_NUMBER(scopeGet(scope, TO_STRING_LITERAL("key"), NULL)) && AS_NUMBER( scopeGet(scope, TO_STRING_LITERAL("key"), NULL) ) == 1);

LN		scopeSetConstant(scope, TO_STRING_LITERAL("key"), TO_NUMBER_LITERAL(2), true);
LN		scope = pushScope(scope);
LN		scope = pushScope(scope);
LN		scope = pushScope(scope);
LN		TEST(IS_NUMBER(scopeGet(scope, TO_STRING_LITERAL("key"), NULL)) && AS_NUMBER( scopeGet(scope, TO_STRING_LITERAL("key"), NULL) ) == 2);

LN		scope = popScope(scope);
LN		TEST(IS_NUMBER(scopeGet(scope, TO_STRING_LITERAL("key"), NULL)) && AS_NUMBER( scopeGet(scope, TO_STRING_LITERAL("key"), NULL) ) == 2);

LN		scope = popScope(scope);
LN		scope = popScope(scope);
LN		scope = popScope(scope);

LN		TEST(IS_NUMBER(scopeGet(scope, TO_STRING_LITERAL("key"), NULL)) && AS_NUMBER( scopeGet(scope, TO_STRING_LITERAL("key"), NULL) ) == 1);

		//extra pushes to test freeing a chain
LN		scope = pushScope(scope);
LN		scope = pushScope(scope);

LN		scope = popScope(scope);
LN		scope = popScope(scope);

		popScope(scope);
	}

	//multiple references to one scope
LN	{
LN		Scope* root = pushScope(NULL);

LN		scopeSetConstant(root, TO_STRING_LITERAL("key"), TO_NUMBER_LITERAL(42), true);

LN		Scope* first = pushScope(root);
LN		Scope* second = pushScope(root);

LN		TEST(root->references == 3); //the root itself + 2 decendants
LN		TEST(first->references == 1);
LN		TEST(second->references == 1);

LN		popScope(root); //doesn't actually get freed, as long as at least 1 descendant is referencing it

LN		TEST(root->references == 2);

LN		popScope(first);
LN		popScope(second);
	}
}