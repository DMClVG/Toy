#ifndef TOY_SCOPE_H
#define TOY_SCOPE_H

#include "common.h"
#include "literal.h"
#include "dictionary.h"
#include "chunk.h"

struct Scope;

typedef struct Scope {
	struct Scope* ancestor;
	Dictionary constants; //a dict of live constants
	Dictionary variables; //a dict of live variables
} Scope;

/*
typedef struct {
	int parameterCount;
	Scope scope;
	Chunk* chunk;
} Function;
*/

Scope* createScope();
void freeScopeChain(Scope* scope);

Scope* pushScope(Scope* scope); //increase the scope count
Scope* popScope(Scope* scope); //decrease the scope count

Literal scopeGet(Scope* scope, Literal key, bool* defined);
bool scopeSetConstant(Scope* scope, Literal key, Literal value, bool declaration);
bool scopeSetVariable(Scope* scope, Literal key, Literal value, bool declaration);

#endif