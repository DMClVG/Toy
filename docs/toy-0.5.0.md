<p align="center">
  <image src="img/toylogo.png" />
</p>

# Toy Language

Version 0.5.0

Welcome to the reference for the Toy programming language!

Toy is a strongly typed stack-based imperative bytecode-interpreted language. It has been revised, and revised, and revised again. Eventually I'll get it right.

## Comments

Toy supports two types of comments, `//single line` and `/* block */`. Comments are used to leave notes for yourself in the code; they are ignored by the interpreter.

## Names

Variable names must start with a letter or underscore, and must contain only letters, numbers and underscores.

## Types

Everything in Toy has a type. These types are:

* null
* bool - the literals `true` and `false`
* int - any whole number
* float - any floating point number
* string - a string of characters enclosed in double quotation marks
* array - a collection of 0-indexed variables
* dict - a collection of indexable key-value pairs
* func - functions are first class citizens

Types are not interoperable, and to change the type of data, you must cast it from it's original type to a new type, like so:

```
x = (type)y
```

Composite types can be created with the `using` keyword, and can be used in place of the basic types:

```
using number = int | float;
```

The `any` type can be used to hold any kind of data, but is discouraged, as it negates the point of a type system.

```
func identity(x: any): any {
	return x;
}
```

## Truthyness

Everything is considered "truthy" except:

* the literal `null`
* the literal `false`
* the integer and float value `0`

## Operators

The following mathematical operators are available - if you want to know what they mean, a quick google search will help:

```
+  -  *  /  %  +=  -=  *=  /=  %=  ++(prefix)  --(prefix)  (postfix)++  (postfix)--
```

Likewise, the following logical operators are available (`&&` is more tightly bound than `||`):

```
(  )  [  ]  {  }  !  !=  ==  <  >  <=  >=  &&  ||  ?:  |
```

Other operators available are the assignment, colon, semicolon and dot operators:

```
= : , .
```

## Entry Point

Every program must have a single entry point; this entry point is a function called "main", which must take an array of strings as it's only argument, and return a single integer, like so:

```
func main(args: string[]): int {
	print "hello world";
	return 0;
}
```

## Var and Const

To declare a variable, use the keyword `var`, followed by it's name, a colon and it's type:

```
var foo: int = 42;
var bar: string = "hello world";
```

Variables can be used in place of literal values at any time, and can be altered and re-assigned. Constants are different - once they are assigned at declaration, they cannot be changed or redefined:

```
const foo: int = 42;
foo++; //Error!
```

Otherwise, constants are just like variables.

## If-Else

The keywords `if` and `else` are used to determine different paths of execution that the program can take, based on a condition. If the condition is truthy, then the `if`-path executes, otherwise the `else`-path does. The else keyword and path are optional.

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

The braces around the different paths are optional if there is only a single statement.

## While and Do-While

The `while` keyword loops over a block of code as long as the condition is truthy:

```
//prints 0-9
var counter: int = 0;
while (counter < 10) {
	print counter++;
}
```

Another way to use the `while` keyword is in the `do`-`while` loop, which is the same as a normal `while` loop, except that it's condition is checked at the end of the block. Note that the `do`-`while` clause must end with a semicolon.

```
//prints 0-10
var counter: int = 0;
do {
	print counter++;
} while (counter < 10);
```

The braces around the body of the `while` and `do`-`while` loops are optional if there is only a single statement.

## For

`while` loops can be quite verbose - a way to write a loop quickly and concisely is with the `for` loop. The first clause in the for loop is executed before the loop begins, then the second clause is checked for truthyness before each execution of the loop body. Finally, the third clause is executed after each execution of the loop body:

```
//prints 0-9
for (var i: int = 0; i < 10; i++) {
	print i;
}
```

Side Note: You *can* declare a `const` in the first clause instead of a `var`, but it won't be very usefull 99.999% of the time.

The braces around the body of the `for` loop are optional if there is only a single statement.

## Foeach-In and Foreach-Of

TODO: These could be useful.

## Break and Continue

During a loop, you may want to exit or restart early for some reason. For this, the `break` and `continue` keywords are provided.

```
//exit a loop early
for (var i: int = 0; i < 10; i++) {
	print i;
	if (i >= 7) {
		break;
	}
}

//restart a loop early
for (var i: int = 0; i < 10; i++) {
	if (i >= 7) {
		continue;
	}
	print i;
}
```

## Functions and Return

Functions are declared with the `func` keyword. They can take any number of arguments, and can return any number of values to the calling context:

```
//this returns nothing
func simple(): null {
	//
}

//this takes an integer as an argument and returns it
func identity(x: any): any {
	return x;
}

//this takes a number of arguments, and returns a number of arguments
func reverseOrder(a: int, b: int, c: int): int, int, int {
	return c, b, a;
}

//this is how you call the above functions
simple();

var result = identity(0);

var a, b, c = reverseOrder(1, 2, 3);
```

TODO: arrow operator to generate inline functions?

Functions can be declared inside other functions, making a closure.

```
func generator(): func {
	var total: int = 0;

	func counter(): int {
		return ++total;
	}

	return counter;
}

var tally = generator();

print tally(); //1
print tally(); //2
print tally(); //3
```

## Classes, Inheritance and Prototypes

No.

## Assert

The `assert` keyword takes 2 parameters, separated by a comma. If the first parameter resolves to be falsy, then the program terminates, and the value of the second parameter is displayed to the user as an error.

```
assert true, "This is fine"; //Good!
assert false, "This is not"; //Error!
```

## Import As

`import` is used to load libraries and external \*.toy files. Several built-in libraries are provided by default. The import keyword can only take a string as it's argument, followed by an optional `as alias`, which stores the results as a variable.

```
import "standard";

print clock(); //the clock function is provided by standard

import "standard" as std;

print std.clock(); //standard is bundled into "std"

import "external.toy"; //runs the external file, and imports the exported variables

import "external2.toy" as External2; //runs the external file, then bundles the exported variables into "External2"
```

## Export As

`export` is the complement to `import` - it's used to expose certain variables to the invoking environment. `as` can rename the exported variable.

```
func hello(): null {
	print "hello world";
}

export hello;
export hello as world;
```

# String

Strings are a series of characters, and are created by surrounding said characters with a pair of double quotation marks `"`. Strings can also be interpolated by replacing the quotation marks with backticks `` ` ``, and inserting variables between `${` and `}`. Characters within a string can be escaped with the `\` character.

```
var greeting: string = "Hello world";

print greeting[0:4]; //Hello
print greeting[::-1]; //dlrow olleH

greeting[0:4] = "Goodnight";

print greeting; //Goodnight world

print greeting.indexOf("oo"); //1
print greeting.length(); //11

var answer: int = 42;
var interpolated: string = `Hello ${val}`; //Hello 42
```

String has the following built in functions, as well as [slice notation](#Indexing-and-Slice-Notation).

## length(): int

This function returns the length of the calling string.

## toLower(): string

This function returns a new string, which is the same as the calling string, except all characters are lower case.

## toUpper(): string

This function returns a new string, which is the same as the calling string, except all characters are upper case.

## replace(pat: string, rep: string): string

For each instance of "pat" that it finds in the calling string, it replaces it with "rep", then returns the new string.

## trim(chars: string): string

Every character in the string "chars" is removed from the calling string's beginning and end, and the new string is returned.

## indexOf(str: string): int

This function returns the position of the first instance of "str" in the calling string.

## lastIndexOf(str: string): int

This function returns the position of the last instance of "str" in the calling string.

## toArray(): string[]

This function returns an array representation of this string, with each character represented by a single string element in the returned array.

# Array

Arrays are collections of variables stored as one piece of data. They can hold any number of values, but "holes" in the array are not permitted. You can access a specific element by using zero-indexing.

```
var arr: string[] = ["hello", "world", "foo", "bar"];

print arr[1]; //world

var arr: int[] = []; //empty
```

## push(x: type): null

This function inserts the value of "x" at the end of the array.

## pop(): type

This function deletes the value at the end of the array, and returns that value.

## unshift(x: type): null

This function inserts the value of "x" at the beginning of the array.

## shift(): type

This function deletes the value at the beginning of the array, and returns that result.

## length(): int

This function returns the length of the array.

## sort(cb(a: type, b: type): int): type[]

This function sorts the elements according to the callback "cb". "cb" may be called any number of times during the sorting process.

"cb" must be a function that takes two parameters, "a" and "b" and returns an integer. If "a" is less than "b", then the returned integer should be negative. If they are equal, then it should be 0. Otherwise it should be positive.

This returns the sorted array, leaving the original variable intact.

```
//how to sort an array of numbers
func sorter(lhs: int, rhs: int): int {
	reuturn a - b;
}

array = array.sort(sorter);
```

## insert(i: int, x: type): null

This function inserts the value of "x" at the index "i", shifting the remaining elements up 1 index.

## delete(i: int): null

This function deletes the value at the index "i", shifting the remianing elements down 1 index.

## containsValue(x: type): bool

This function returns true if the array contains an element that is equal to "x".

## every(cb(x: type): bool): bool

This function calls "cb" once for every element in the array, with that element passed in as "x". If any call to "cb" returns false, then the function exits early and returns false. Otherwise the function returns true.

## any(cb(x: type): bool): bool

This function calls "cb" once for every element in the array, with that element passed in as "x". If any call to "cb" returns true, then the function exits early and returns true. Otherwise the function returns false.

## filter(cb(x: type): bool): type[]

This function calls "cb" once for every element in the array, with that element passed in as "x". This returns a new array that contains every element for which the call to "cb" returned true.

## map(cb(x: type): type2): type2[]

This function calls "cb" once for every element in the array, with that element passed in as "x". It returns a new array instance with the result of each call to "cb" as it's elements.

## reduce(default: type2, cb(acc: type2, x: type): type2): type2

This function calls "cb" once for every element in the array, with that element passed in as "x", and the value of the previous call passed in as "acc". For the first call to "cb", "default" is used for "acc". The final value of "acc" is returned.

## concat(x: type[]): type[]

This function requires an array of the same type as "x". This function returns a new array which contains the contents of the current array followed by the contents of "x".

## clear(): null

This function removes the contents of the array, leaving an empty array.

## equals(x: type[]): bool

This function requires an array as "x". It returns false if the current array and "x" have a different number of elements. Then, it iterates over each pair of elements from the current array and "x" and compares them, returning false on the first mismatch found. Otherwise it returns true.

## toString(): string

This function returns a string representation of the array. Each element is converted to a string, and separated by commas. Finally, the whole string is surrounded by brackets.

Nesting an array within it's own data structure will cause the inner reference to be printed as "\<circular reference\>".

# Dictionary

Dictionaries are key-value collections of variables. Every key has a single associated value; any missing value is considered `null`.

```
var dictionary: dict[string, string] = ["key one": "value one", "key two": "value two"];

print dictionary["key one"]; //value one

var dictionary: dict[int, int] = []; //empty
```

## Indexing

Dictionaries can be indexed using traditional bracket notation, or the dot operator when the keys are strings. Existing elements can be accessed or overwritten, or new ones inserted if they don't already exist this way.

```
dictionary["foo"] = "bar";
print dictionary["foo"];
print dictionary.foo; //syntactic sugar (can't be used for assignment)
```

## insert(k: type, v: type2): int

This function inserts a new key-value pair "k:v", or returns non-zero if that key already exists.

## delete(k: type): int

This function deletes the key-value pair with the key "k". Returns non-zero if the key doesn't exist.

## length(): int

This function returns the number of key-value pairs in a dictionary.

## containsKey(k: type): bool

This function returns true if the dictionary contains a key "k", otherwise it returns false.

## containsValue(v: type): bool

This function returns true if the dictionary contains a value "v", otherwise it returns false.

## every(cb(k: type, v: type2): bool): bool

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. If any call to "cb" returns false, then the function exits early and returns false. Otherwise the function returns true.

## any(cb(k: type, v: type2): bool): bool

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. If any call to "cb" returns false, then the function exits early and returns true. Otherwise the function returns false.

## filter(cb(k: type, v: type2): bool): dict[type, type2]

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. This returns a new dictionary that contains every key-value pair for which the call to "cb" returned true.

## map(cb(k: type, v: type2): type3): dict[type, type3]

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully. It returns a new dictionary with keys copied from the current dictionary, and values replaced with the results of the calls to "cb".

## reduce(default: type3, cb(acc: type3, k: type, v: type2): type3): type3

This function calls "cb" once for every pair in the dictionary, with that key and value passed in as "k" and "v", respectfully, and the value of the previous call passed in as "acc". For the first call to "cb", "default" is used for "acc". It returns the final value of "acc".

## concat(x: dict[type, type2]): dict[type, type2]

This function requires a dictionary as "x". This function returns a new dictionary instance which contains the contents of the current dictionary combined with the contents of "x". In the envent of a key clash, the key-value pair in the current dictionary is included, and the key-value pair from "x" is discarded.

## clear()

This function removes the contents of the dictionary instance, leave an empty dictionary.

## equals(x: dict[type, type2]): bool

This function requires a dictionary instance as "x". It returns false if the current dictionary and "x" have a different number of key-value pairs. Then, it iterates over each key-value pair in the current dictionary and checks "x" for a matching key, returning false if a key is missing. Then, before moving onto the next key-value pair, it compares the values with that key in each dictionary and returns false if a mismatch is found.

Finally, this function returns true.

## keys(): type[]

This function returns an array containing each key in the dictionary. The order of the keys is undefined.

## values(): type2[]

This function returns an array containing each value in the dictionary. The order of the values is undefined.

## toString()

This function returns a string representation of the dictionary. Each key-value pair is converted to a string with a colon between the two, and each pair is separated by commas. Finally, the whole string is surrounded by brackets.

Nesting a dictionary within it's own data structure will cause the inner reference to be printed as "\<circular reference\>".

# Other Stuff

## Indexing and Slice Notation

Strings, arrays and dictionaries can be indexed in several ways. Elements can be accessed using traditional bracket notation:

```
str[0];

arr[x];

dict["key"];
```

However, "slice notation" is also available for strings and arrays:

```
str[x:y];
```

Here, "x" and "y" are integers that indicate two indexes within a string, and returns a new substring beginning at "x" and ending at "y" (inclusive). If either is omitted, then the first and last element positions respectfully are used. The same applies for the array type.

Replacing parts of a string with another string, or part of an array with another array, is possible using slice notation:

```
var str: string = "hello world";
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

## Error Handling

TODO: CBF - just crash for now.

