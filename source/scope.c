#include "scope.h"
#include "memory.h"

#include "function.h"

Scope* pushScope(Scope* ancestor) {
	Scope* scope = ALLOCATE(Scope, 1);
	scope->ancestor = ancestor;
	initDictionary(&scope->constants);
	initDictionary(&scope->variables);

	scope->references = 0;
	for (Scope* ptr = scope; ptr; ptr = ptr->ancestor) {
		ptr->references++;
	}

	return scope;
}

//run up the ancestor chain, freeing anything with 0 references left
static void freeScopeChain(Scope* scope) {
	scope->references--;

	//free scope chain
	if (scope->ancestor != NULL) {
		freeScopeChain(scope->ancestor);
	}

	if (scope->references > 0) {
		return;
	}

	freeDictionary(&scope->constants);
	freeDictionary(&scope->variables);

	FREE(Scope, scope);
}

Scope* popScope(Scope* scope) {
	Scope* ret = scope->ancestor;

	//TODO: foreach
	//search for functions that reference the scope, and unref them
	for (int i = 0; i < scope->constants.count; i++) {
		if (IS_FUNCTION(scope->constants.entries[i].value)) {
			//detach the function scopes before they're free'd
			unreferenceScope(AS_FUNCTION_PTR(scope->constants.entries[i].value)->scope);
			AS_FUNCTION_PTR(scope->constants.entries[i].value)->scope = NULL;
		}
	}

	for (int i = 0; i < scope->variables.count; i++) {
		if (IS_FUNCTION(scope->variables.entries[i].value)) {
			//detach the function scopes before they're free'd
			unreferenceScope(AS_FUNCTION_PTR(scope->variables.entries[i].value)->scope);
			AS_FUNCTION_PTR(scope->variables.entries[i].value)->scope = NULL;
		}
	}

	freeScopeChain(scope);

	return ret;
}

Scope* referenceScope(Scope* scope) {
	for (Scope* ptr = scope; ptr; ptr = ptr->ancestor) {
		ptr->references++;
	}

	return scope;
}

Scope* unreferenceScope(Scope* scope) {
	if (scope->references == 1) {
		freeScopeChain(scope);
		return NULL;
	}

	for (Scope* ptr = scope; ptr; ptr = ptr->ancestor) {
		ptr->references--;
	}

	return scope;
}

Literal scopeGet(Scope* scope, Literal key, bool* defined) {
	if (defined) {
		*defined = true;
	}
	if (dictionaryDeclared(&scope->constants, key)) {
		return dictionaryGet(&scope->constants, key);
	}

	if (dictionaryDeclared(&scope->variables, key)) {
		return dictionaryGet(&scope->variables, key);
	}

	if (scope->ancestor != NULL) {
		return scopeGet(scope->ancestor, key, defined);
	} else {
		if (defined) {
			*defined = false;
		}
		return TO_NIL_LITERAL;
	}
}

bool scopeSetConstant(Scope* scope, Literal key, Literal value, bool declaration) {
	if (declaration) {
		//check for existing constants & variables with that key
		if (dictionaryDeclared(&scope->constants, key) || dictionaryDeclared(&scope->variables, key)) {
			return false;
		}

		dictionarySet(&scope->constants, key, value);
		return true;
	} else {
		return false;
	}
}

bool scopeSetVariable(Scope* scope, Literal key, Literal value, bool declaration) {
	if (declaration) {
		//check for existing constants & variables with that key
		if (dictionaryDeclared(&scope->constants, key) || dictionaryDeclared(&scope->variables, key)) {
			return false;
		}

		dictionarySet(&scope->variables, key, value);
		return true;
	} else {
		while (scope != NULL) {
			//can't assign to a constant
			if (dictionaryDeclared(&scope->constants, key)) {
				return false;
			}

			if (dictionaryDeclared(&scope->variables, key)) {
				dictionarySet(&scope->variables, key, value);
				return true;
			}

			//walk up the chain
			scope = scope->ancestor;
		}

		return false;
	}
}

int getScopeDepth(Scope* scope) {
	if (scope->ancestor == NULL) {
		return 0;
	}

	return getScopeDepth(scope->ancestor) + 1;
}