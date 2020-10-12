# Toy Language

Version 0.4.0

Welcome to the reference for the Toy programming language!

I'm still developing this language and discovering all of it's incedental nooks and crannys, but hopefully the core of the language is solid enough that I can begin to teach people to use it. This guide on how to use the language will also double as the official documentation for the time being.

If you're familiar with JavaScript, you will see a lot of similarities. However, there are also differences that I hope will make my language stand out in time. I don't expect it to reach JavaScript's popularity, but I would like it to be useful.

# Notes

Anything marked as TODO is not part of the language - but may be implemented at some point in the future.

# Basics

## Print

Let's start with the most fundemental keyword - `print`.

```
print "hello world";
```

`print` is followed by a single expression that resolves to either a literal or a variable, which is converted to a string and then outputted to the console, followed by a newline.

`print` is a keyword, rather than a library function. This is partially due to Toy's origin as a [lox derivative](http://craftinginterpreters.com/), but I've decided to keep it, as it makes it easy to debug code without requiring a library of some kind (and all of the baggage that comes with). You *can* use it with parentheses, but they're not required at all.

## Comments

Toy supports two types of comments, `//single line` and `/* block */`. Comments are used to leave notes for yourself in the code; they are ignored by the interpreter.

```
var a = "foo"; //this is a comment

/*
so is this
*/
```

## Datatypes

These are the literal datatypes currently built into the language:

* `null`
* Boolean - the literals `true` and `false`
* Number - any double-precision floating point number
* [String](#String) - a string of characters enclosed in quotation marks `""`
* [Array](#Array) - a collection of 0-indexed variables
* [Dictionary](#Dictionary) - a collection of key-value pairs
* [Function](#Functions-and-Return) - functions are data
* [Event](#Event) - Events are collections of functions

## Truthyness

Everything is considered "truthy" except:

* the literal `null`
* the literal `false`
* the number 0

## Operators

The following mathematical operators are available - if you want to know what they mean, a quick google search will help:

```
+  -  *  /  %  +=  -=  *=  /=  %=  ++(prefix)  --(prefix)  (postfix)++  (postfix)--
```

Likewise, the following logical operators are available:

```
!  !=  ==  <  >  <=  >=  &&  ||  ?:
```

As well as the assignment operator, the method operator (Methods are functions attached to objects), and the member-of operator (see [Dictionary](#Dictionary)):

```
=  ::  .
```

Note that:

* All of the mathematical operators can be used on numbers
* `+` and `+=` can be used on strings for concatenation
* `==` and `!=` can be used on strings or numbers to compare them, but not strings AND numbers; Functions and events only equal themselves
* `+=` and `-=` can be used on events to subscribe and unsubscribe a callback, respectfully

Remember, `&&` is more tightly bound than `||`.

There is also the pipe operator: `|>`, which replaces a series of nested calls:

```
//replace this:
print increment(double(increment(double(5)))); //23

//with this:
print 5 |> double |> increment |> double |> increment; //23
```

And the backpipe operator: `<|`, which calls one function many times:

```
const say = arg => {
	print arg;
};

//replace this:
say("Hello world!");
say("It's dangerous to go alone!");
say("Take this!");

//with this:
say
	<| "Hello world!"
	<| "It's dangerous to go alone!"
	<| "Take this!"
	;
```

## Indexing and Slice Notation

Strings, Arrays and Dictionaries can be indexed in several ways. Elements can be accessed using traditional bracket notation:

```
str[x];

arr[y];

dict["key"];
```

However, "slice notation" is also available for strings and arrays:

```
str[x:y];
```

Here, "x" and "y" indicate two indexes within a string, and returns a new substring beginning at "x" and ending at "y" (inclusive). If either is omitted, then the first and last element positions respectfully are used. The same applies for the Array type.

Replacing parts of a string with another string, or part of an array with another array, is possible using slice notation:

```
str = "hello world";
str[6:11] = "user";
print str; //hello user
```

A third argument is possible:

```
str[x:y:z];
```

Here, "z" indicates how to count the elements - a positive number starts from the beginning of the string or array, while a negative one counts from the end. Also, if a number other than 1 or -1 is used, then every nth element is selected:

```
print str; //Hello world
print str[::2]; //Hlowrd
print str[::-2]; //drwolH
```

0 cannot be used as the third argument.

## Var and Const

Side Note: For those familiar with JavaScript, know that there is no variable hoisting in Toy. If you don't know what this means, ignore this.

To declare a variable, use the keyword `var`:

```
var foo = 42;
var bar = "hello world";
```

Variables can be used in place of literals at any time, and can be altered and re-assigned. Constants are different - once they are assigned at declaration, they cannot be changed or redefined:

```
const foo = 42;
foo++; //Error!
```

Otherwise, constants are just like variables.

## If-Else

The keywords `if` and `else` are used to determine different paths that the program can take, based on a condition. If the condition is truthy, then the `if`-path executes, otherwise the `else`-path does. The else keyword and path are optional.

```
if (1 < 2) {
	print "this will print to the console";
} else {
	print "this will not";
}
```

multiple `if`-`else` statements can be chained:

```
if (value == 1) {
	print "one";
} else if (value == 2) {
	print "two";
} else if (value == 3) {
	print "three";
} else {
	print "unknown value";
}
```

The braces around the different paths are optional, but if they are omitted, they will be implicitly inserted.

## While and Do-While

The `while` keyword loops over a block of code as long as the condition is truthy:

```
var counter = 0;
while (counter < 10) {
	print counter;
	counter++;
}
```

Another way to use the `while` keyword is in the `do`-`while` loop, which is the same as a normal `while` loop, except that it is executed at least once before the condition is checked. Note that the `do`-`while` clause must end with a semicolon.

```
var counter = 0;
do {
	print counter;
	counter++;
} while (counter < 10);
```

The braces around the body of the `while` and `do`-`while` loops are optional, but if they are omitted, they will be implicitly inserted.

## For

`while` loops can be quite verbose - a way to write a loop quickly and concisely is with the `for` loop. The first clause in the for loop is executed before the loop begins, then the second clause is checked for truthyness before each execution of the loop body. Finally, the third clause is executed after each execution of the loop body:

```
for (var i = 0; i < 10; i++) {
	print i;
}
```

Side Note: You *can* declare a `const` in the first clause instead of a `var`, but it won't be very usefull 99.999% of the time.

The braces around the body of the `for` loop are optional, but if they are omitted, they will be implicitly inserted.

## Switch, Case and Default

The `switch` keyword can be used to jump to a specific `case` statement based on the value of a variable. The [`break`](#Break-and-Continue) keyword can be used to break out of such a case. Note that non-empty fall-through cases are considered an error. Switch statements can operate on `null`, booleans, numbers and strings, however the case statements must use a literal.

The default keyword can be inserted at the end of the switch block to catch any unmatched values.

```
var variable = "foo";

switch(variable) {
	case "foo":
		print "hello foo";
		break; //jumps to the end of the block

	case "bar": //example of a fall-through
	case "bazz":
		print "hello stranger";
		break;

	default:
		print "error, unknown variable";
}
```

## Foeach-In and Foreach-Of

TODO: Iterating over arrays and dictionaries? This can be done with `.ForEach()`, and I'd like there to be only one way of doing each thing.

## Break and Continue

During a loop, you may want to exit or restart early for some reason. For this, the `break` and `continue` keywords are provided.

```
//exit a loop early
for (var i = 0; i < 10; i++) {
	print i;
	if (i >= 7) {
		break;
	}
}

//restart a loop early
for (var i = 0; i < 10; i++) {
	if (i >= 7) {
		continue;
	}
	print i;
}
```

TODO: `break x;` and `continue x;` i.e. breaking out of multiple nested loops using a label? Scoped labels and goto?

## Functions and Return

Unlike many other mainstream languages, functions are not statements, but expressions. They are also ONLY first class functions, and do not have names attached; they must be stored in a variable or constant like any other datatype. Note that since functions are just expressions, a function declaration MUST end with a semicolon.

Declaring a function is done using the arrow operator.

```
const f = () => {
	print "bar";
};
```

A function can be called by appending a pair of parentheses after it, and arguments can be passed to it by placing values between those parentheses, separated by commas. An incorrect number of arguments passed is a runtime error.

```
const f = (arg) => {
	print arg;
};

f("foo"); //Good!
f(); //Error!
f("foo", "bar"); //Error!
```

When a function is declared, if there is exactly 1 argument, then the parentheses can be omitted. Similarly, if there is only 1 expression statement in the function body, then the braces can be omitted, which causes the result of the expression statement to be returned.

```
const caller = (cb) => {
	return cb("arg");
}

caller(x => x); //very concise identity function

array.Sort((a, b) => a - b); //a more practical example
```

Values can be extracted from function calls using the `return` keyword:

```
const f = () => {
	return "foo";
};

var bar = f(); //bar = "foo"
```

Closures are explicitly supported, so inaccessible, or "private", variables can be created this way:

```
const makeCounter = () => {
	var counter = 0; //the inaccessible variable
	return () => {
		return ++counter;
	};
};

const counter = makeCounter();

print counter(); //1
print counter(); //2
print counter(); //3
```

Very complex structures can be created using functions this way, including mimicing classes.

## Classes, Inheritance and Prototypes

No.

## Assert

The `assert` keyword takes 2 parameters, separated by a comma. If the first parameter resolves to be falsy, then the program terminates, and the value of the second parameter is displayed to the user.

```
assert true, "This is fine"; //Good!
assert false, "This is not"; //Error!
```

## Import As

`import` is used to load libraries and external \*.toy files. Several built-in libraries are provided by default. The import keyword can only take a string as it's argument, followed by an optional `as alias`.

```
import "Standard";

print Clock(); //the clock function is provided by Standard

import "Standard" as std;

print std.Clock(); //Standard is bundled into "std"

import "external.toy"; //runs the external file, and merges the environment into this one

import "external2.toy" as External2; //runs the external file, then stores the environment into a dictionary named External2
```

# String

Strings are a series of characters, and are created by surrounding said characters with a pair of quotation marks `"`. Strings can also be interpolated by replacing the quotation marks with backticks `` ` ``, and inserting variables between `${` and `}`. Characters within a string can be escaped with the `\` character.

```
var greeting = "Hello world";

print greeting[0:4]; //Hello
print greeting[::-1]; //dlrow olleH

greeting[0:4] = "Goodnight";

print greeting; //Goodnight world

print greeting::IndexOf("oo"); //1
print greeting::Length(); //11

var answer = 42;
var interpolated = `Hello ${val}`; //Hello 42
```

String has the following built in functions, as well as [slice notation](#Indexing-and-Slice-Notation).

## Length()

This function returns the length of the calling string.

## ToLower()

This function returns a new string, which is the same as the calling string, except all characters are lower case.

## ToUpper()

This function returns a new string, which is the same as the calling string, except all characters are upper case.

## Replace(pat, rep)

For each instance of "pat" that it finds in the calling string, it replaces it with "rep", then returns the new string.

## Trim(chars)

Every character in the string "chars" is removed from the calling string's beginning and end, and the new string is returned.

## IndexOf(str)

This function returns the position of the first instance of "str" in the calling string.

## LastIndexOf(str)

This function returns the position of the last instance of "str" in the calling string.

## ToArray()

This function returns an array representation of this string, with each character represented by a single string element in the returned array.

# Array

Arrays are collections of variables stored as one piece of data. They can hold any number of values, but "holes" in the array are not permitted. You can access a specific element by using zero-indexing. Arrays can be "destructured" into individual variables.

```
var arr = ["hello", "world", "foo", "bar"];

print arr[1]; //world

var [a, b] = arr; //destructuring

var arr = []; //empty
```

## Push(x)

This function inserts the value of "x" at the end of the array.

## Pop()

This function deletes the value at the end of the array, and returns that value.

## Unshift(x)

This function inserts the value of "x" at the beginning of the array.

## Shift()

This function deletes the value at the beginning of the array, and returns that result.

## Length()

This function returns the length of the array.

## Sort(cb(a, b))

This function sorts the elements according to the callback "cb". "cb" may be called any number of times during the sorting process.

"cb" must be a function that takes two parameters, "a" and "b" and returns a number. If "a" is less than "b", then the returned number should be negative. If they are equal, then it should be 0. Otherwise it should be positive.

```
//how to sort an array of numbers
array::Sort((a, b) => a - b);
```

## Insert(i, x)

This function inserts the value of "x" at the index "i", shifting the remaining elements up 1 index.

## Delete(i)

This function deletes the value at the index "i", shifting the remianing elements down 1 index.

## ContainsValue(x)

This function returns true if the array contains the value of "x".

## Every(cb(x))

This function calls "cb" once for every element in the array, with that element passed in as "x". If any call to "cb" returns false, then the function exits early and returns false. Otherwise the function returns true.

## Any(cb(x))

This function calls "cb" once for every element in the array, with that element passed in as "x". If any call to "cb" returns true, then the function exits early and returns true. Otherwise the function returns false.

## Filter(cb(x))

This function calls "cb" once for every element in the array, with that element passed in as "x". This returns a new array instance that contains every element for which the call to "cb" returned true.

## ForEach(cb(x))

This function calls "cb" once for every element in the array, with that element passed in as "x".

## Map(cb(x))

This function calls "cb" once for every element in the array, with that element passed in as "x". It returns a new array instance with the result of each call to "cb" as it's elements.

## Reduce(default, cb(acc, x))

This function calls "cb" once for every element in the array, with that element passed in as "x", and the value of the previous call passed in as "acc". For the first call to "cb", "default" is used for "acc".

## Concat(x)

This function requires an array instance as "x". This function returns a new array instance which contains the contents of the current array followed by the contents of "x".

## Clear()

This function removes the contents of the array instance, leaving an empty array.

## Equals(x)

This function requires an array instance as "x". It returns false if the current array and "x" have a different number of elements. Then, it iterates over each pair of elements from the current array and "x" and compares them, returning false on the first mismatch found. Otherwise it returns true.

## ToString()

This function returns a string representation of the array. Each element is converted to a string, and separated by commas. Finally, the whole string is surrounded by brackets.

Nesting an array within it's own data structure will cause the inner reference to be printed as "\<circular reference\>".

# Dictionary

Dictionaries are key-value collections of variables. Every key has a single associated value; any missing value is considered `null`.

```
var dict = ["key one" : "value one", "key two" : "value two"];

print dict["key one"]; //value one

var dict = [:]; //empty
```

## Indexing

Dictionaries can be indexed using traditional bracket notation, or the member-of operator. Existing elements can be accessed or overwritten, or new ones inserted if they don't already exist this way.

```
dictionary["foo"] = "bar";
print dictionary["foo"];
print dictionary.foo; //syntactic sugar
```

## Insert(k, v)

This function inserts a new key-value pair "k:v", or the program fails if that key already exists.

## Delete(k)

This function deletes the key-value pair with the key "k". Nothing happens if the key-value pair doesn't exist.

## Length()

This function returns the number of key-value pairs in a dictionary.

## ContainsKey(k)

This function returns true if the dictionary contains a key "k", otherwise it returns false.

## ContainsValue(V)

This function returns true if the dictionary contains a value "v", otherwise it returns false.

## Every(cb(k, v))

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. If any call to "cb" returns false, then the function exits early and returns false. Otherwise the function returns true.

## Any(cb(k, v))

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. If any call to "cb" returns false, then the function exits early and returns true. Otherwise the function returns false.

## Filter(cb(k, v))

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. This returns a new dictionary instance that contains every key-value pair for which the call to "cb" returned true.

## ForEach(cb(k, v))

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. 

## Map(cb(k, v))

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. It returns a new dictionary instance with keys copied from the current dictionary, and values replaced with the results of the calls to "cb".

## Recuce(default, cb(acc, k, v))

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully, and the value of the previous call passed in as "acc". For the first call to "cb", "default" is used for "acc".

## Concat(x)

This function requires a dictionary instance as "x". This function returns a new dictionary instance which contains the contents of the current dictionary combined with the contents of "x". In the envent of a key clash, the key-value pair in the current dictionary is included, and the key-value pair from "x" is discarded.

## Clear()

This function removes the contents of the dictionary instance, leave an empty dictionary.

## Equals(x)

This function requires a dictionary instance as "x". It returns false if the current dictionary and "x" have a different number of key-value pairs. Then, it iterates over each key-value pair in the current dictionary and checks "x" for a matching key, returning false if a key is missing. Then, before moving onto the next key-value pair, it compares the values with that key in each dictionary and returns false if a mismatch is found.

Finally, this function returns true.

## Keys()

This function returns an array containing each key in the dictionary. The order of the keys is undefined.

## Values()

This function returns an array containing each value in the dictionary. The order of the values is undefined.

## ToString()

This function returns a string representation of the dictionary. Each key-value pair is converted to a string with a colon between the two, and each pair is separated by commas. Finally, the whole string is surrounded by braces.

Nesting a dictionary within it's own data structure will cause the inner reference to be printed as "\<circular reference\>".

# Event

```
var evt = [..];

var unsub = evt::Subscribe(x => print x);

evt += x => { print x; };

evt("hello world");
```

Events are most often treated like functions with a single argument. To call an event, simply invoke it's name, and pass it an argument.

## Subscribe(cb(x))

This function collects and stores "cb" into an internal list of callbacks, which are each called when the event is invoked. This function returns a function which can be called to remove "cb" from that list. Any number of callbacks can be stored as many times as needed.

## Message(x)

This function calls all stored callbacks with the argument "x". The order of the calls is undefined.

## Clear()

This function removes all callbacks from the internal list of callbacks. It is called automatically when the event goes out of scope or is deleted.

# Random Ideas

* Dictionary destructuring?
* Pure Keyword
* Capture syntax for functions
* Coroutines
* Access individual functions within an event

```
//capture syntax
const f = (args) => [x] {
	print args + x;
};

//pure syntax
const f = (args) => pure {
	return args; //"print" keyword is not allowed in pure functions
};

//coroutines
cosnt cr = delay => coroutine {
	yield WaitForSeconds(delay);
	print "Hello world";
};

Coroutine::Run(cr(1));

//I think it might internally wrap the coroutine in an extra layer function like this:
const cr = arg => Coroutine(() => { /* code */ });

//So `Coroutine` would be a type that holds a function, and the `coroutine` keyword does invisible wrapping...
const cr = delay => {
	return Coroutine(() => {
		yield WaitForSeconds(delay);
		print "hello world";
	});
};
```

## Rejected Ideas

* async/await/promises - no way to signal an error

```
//async syntax
const f = (args) => async {
	print args;
};

const g = (args) => async {
	await f(args);
};

Promise::All([g]); //blocks until all functions in the array return
```
