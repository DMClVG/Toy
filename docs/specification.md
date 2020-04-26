# Objects

There's no difference between a class and an object; the same instance can be used as both.

An object is defined with the object operator: `->`

```
const CClass = (parameter) -> {
	const Method = (arg) => {
		return arg + parameter;
	};

	const GetParameter = () => {
		return parameter;
	}

	const SetParameter = x => {
		return parameter = x;
	}
};

const object = CClass(2);

print object.Method(3); //will print 5
```

Objects have no constructor or destructor - instead, the resulting object receives parameters from the invocation.

```
var object = CClass(3); //invoking CClass to create object with a "parameter" of 3
```

While members are normally public, and invocation parameters are normally private, you can expose invocation parameters using the "expose" keyword:

```
const CClass = (parameter) -> {
	expose parameter;
};
```

Likewise, you can hide a member using the "private" keyword:

```
const CClass = () -> {
	private const member = null;
};
```

using both "expose" and "private" on the same member is an error.

It's possible to define the invocation of an instance object using the child invoker, which is a single object in the body that is not bound to a name:

```
const CParent = (parameter1) -> {
	(parameter2, parameter3) -> { //child invoker
		const Concat() => {
			return parameter1 + parameter2 + parameter3;
		};
	};
};

const object = CParent("parameter1");

const child = object("parameter2", "parameter3");

print child.Concat();
```

Invoking an object without a child invoker is an error.

You can inherit from an existing object using the "extend" keyword. This will clone the original object into the new object:

```
const CParent = (parameter) -> {
	//all members have access to "parameter"

	const Method = x => x + parameter;
};

const CClass = extend CParent with () -> {
	//all members have access to copies of "parameter" and "Method"
};

const object = CClass("parameter");

print object.Method("x");

```

You can also implement another object. If an object implements another, it must:

* invoke with the same number of arguments  
* have an identical, but extensible child invoker (with identical parents, interfaces and child invokers, recursively)  
* have identical public members  

```
const IInterface = (member1) -> {
	() -> {
		const Method = x => null;
	};

	const Method = x => null;
};

const CClass = implement IInterface with (parameter) => {
	() -> {
		const Method = arg => "can return anything";
		const Method2 = () => "can have other methods too";
	};

	const Method = arg => "can return anything";
	const Method2 = () => "can have other methods";
};

const object = CClass("parameter");
const child = object(); //use the child invoker

print child.Method(); //prints "can have other methods too"
```

If a class has both a parent with invoker parameters, and an interface with invoker parameters, the parent's parameters count towards the interface's requirements. Likewise, any child invokers also count towards the interface's requirements:

```
const CParent = (parameter) -> {
	() -> {
		const Method = x => null;
	};
};

const IInterface = (parameter) -> {
	() -> {
		const Method = x => null;
	};

	const Method = x => null;
};

const CClass = extend CParent implement IInterface with () => {
	const Method = arg => null;
};

const obj = CClass("parameter");
const child = obj();

print child.Method(42); //returns null
```

Interfaces do not interact with each other, as they must be fufulled separately on the same object.

Objects can be defined with optional parenthesis if they have exactly 1 parameter, and optional braces if they have only one member or child invoker.

```
const CChild = extend CParent implement IInterface with parameter -> const Method = arg => null;
```

You can access the current object using the "this" keyword, or the parent using "parent". You can also compare object types using:

* extends - does a extend b?  
* implements - does a implement b?  
* instantiates - does a instantiate b?  

```
const CParent = () -> {
	//
};

const IInterface = () -> {
	//
};

const IInterface2 = () -> {
	//
};

const CClass = extend CParent implement IInterface, IInterface2 with () -> {
	implement IInterface2 with () -> {
		//
	};
};

const object = CClass();
const child = object();

//tests
print CClass extends CParent; //true
print CClass implements IInterface; //true
print object instantiates CClass; //true
print child instantiates object; //true
print child implements IInterface2; //true
```

Note: An object definition affects only the instances of that object, not the object itself. Thus, extending from an object won't clone the object's members, only it's definition.

```
const CClass = () -> {
	const Method = () => null;
};

CClass.Method(); //ERROR!
```

Note: "this" and "parent" members can both be exposed.

Note: "definition" is a reserved keyword. This may in future hold the definition of an object as a string, which could be used by some kind of "eval" function, and manipulated by the program.

