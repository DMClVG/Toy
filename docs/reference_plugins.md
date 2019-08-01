# Plugins

This is a detailed look at the plugins that are bundled with the Toy lanugage. For information on how to create your own plugins, see [the plugins tutorial](tutorial_plugins.md).

# Array

Instances of the Array plugin hold arrays which are indexed by numbers, beginning from 0 and counting up. Arrays do not allow for "holes" i.e. missing elements.

## Usage

```
import "Array";

var array = Array();

array.Push(1);
array.Push(2);
array.Push(3);

print array[1]; //"2"

var copy = array[:]; //copy an array
var backwards = array[::-1]; //reverse the array

array[1] = "foo"; //assign to an element
```

## Indexing

Arrays can be indexed in several ways. Elements can be accessed using traditional bracket notation:

```
array[x]
```

However, "slice notation" is also available:

```
array[x:y];
```

Here, "x" and "y" indicate two indexes within the array, and returns a new array containing only the elements between those two indexes (inclusive). If either is omitted, then the first and last elements respectfully are used.

A third argument is possible:

```
array[x:y:z];
```

Here, "z" indicates how to count the elements - a positive number starts from the beginning of the array, while a negative one counts from the end. Also, if a number other than 1 or -1 is used, then every nth element is selected:

```
print array; //output: [1,2,3,4,5,6,7,8,9,10]
print array[::2]; //output: [1,3,5,7,9]
print array[::-2]; //output: [10,8,6,4,2]
print array[::-2][::-1]; //output: [2,4,6,8,10]
```

0 cannot be used as the third argument.

## Push(x)

This function inserts the value of x at the end of the array.

## Pop()

This function deletes the value at the end of the array, and returns the value.

## Unshift(x)

This function inserts the value of x at the beginning of the array.

## Shift()

This function deletes the value at the beginning of the array, and returns the result.

## Length()

This function returns the length of the array.

## Sort(cb)

This function sorts the elements according to the callback "cb". "cb" may be called any number of times during the sorting process.

"cb" must be a function that takes two parameters, "a" and "b" and returns a number. If "a" is less than "b", then the returned number should be negative. If they are equal, then it should be 0. Otherwise it should be positive.

```
//how to sort an array of numbers
//note that if there are any non-numbers, then the program will fail
array.Sort((a, b) => a - b);
```

## Insert(i, x)

This function inserts the value of x at the index i, shifting the remaining elements up 1 index.

## Delete(i)

This function deletes the value at the index i, shifting the remianing elements down 1 index.

## ToString()

This function returns a string representation of the array. Each element is converted to a string, and separated by commas. Finally, the whole string is surrounded by brackets.

Nesting an array within it's own data structure will cause the inner reference to be printed as "\<circular reference\>".

# Dictionary

Instances of the Dictionary plugin hold dictionaries, which are are collections of key-value pairs.

## Usage

```
import "Dictionary";

var dictionary["key"] = "value";

dictionary.Insert("foo", "bar");
dictionary.Delete("key");

print dictionary.Contains("foo"); //true
```

## Indexing

Dictionaries can be indexed using traditional bracket notation. Existing elements can be accessed or overwritten, or new ones inserted if they don't already exist this way:

```
dictionary["foo"] = "bar";
print dictionary["foo"];
```

## Insert(k, v)

This function inserts a new key-value pair "k:v", or the program fails if that key already exists.

## Delete(k)

This function deletes the key-value pair with the key "k". Nothing happens if the key-value pair doesn't exist.

## Length()

This function returns the number of key-value pairs in a dictionary.

## Contains(k)

This function returns true if the dictionary contains a key "k", otherwise it returns false.

## ToString()

This function returns a string representation of the dictionary. Each key-value pair is converted to a string with a colon between the two, and each pair is separated by commas. Finally, the whole string is surrounded by braces.

Nesting a dictionary within it's own data structure will cause the inner reference to be printed as "\<circular reference\>".

