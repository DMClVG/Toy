#include "scope.h"
#include "memory.h"

#include <stdio.h>

Scope* createScope() {
	Scope* scope = ALLOCATE(Scope, 1);
	scope->ancestor = NULL;
	initDictionary(&scope->constants);
	initDictionary(&scope->variables);
	return scope;
}

void freeScopeChain(Scope* scope) {
	//free scope chain
	if (scope->ancestor != NULL) {
		freeScopeChain(scope->ancestor);
	}

	freeDictionary(&scope->constants);
	freeDictionary(&scope->variables);

	FREE(Scope, scope);
}

Scope* pushScope(Scope* scope) {
	Scope* ret = ALLOCATE(Scope, 1);

	ret->ancestor = scope;
	initDictionary(&ret->constants);
	initDictionary(&ret->variables);

	return ret;
}

Scope* popScope(Scope* scope) {
	Scope* ret = scope->ancestor;

	freeDictionary(&scope->constants);
	freeDictionary(&scope->variables);
	FREE(Scope, scope);

	return ret;
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
