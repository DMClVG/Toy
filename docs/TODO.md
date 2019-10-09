//TODO: function pipes |>, function cascading .., function concatenation + (cascading with a single variable-set)

//pipes: |> (must have only one argument - DONE!)
//	var val = 5 |> double |> increment; //val = 11
//backpipe: <| (syntactic sugar?)
//  say <| "hello" <| "world";

//concatenation: f + g + h (concatenated functions don't have return values, and don't pipe)
//	var log = x => { print x; };
//	var process = x => ...;
//	var postProcess = x => ...;
//	var eventHook = x => ...;

//	var f = log + process + postProcess + eventHook;
//
//	//no returns
//	f("argument");

//cascading: .. (not useful without classes - WON'T ADD)
//	var str = "hello world";
//	str.DoThingToString()
//		..DoOtherThingToString()
//		;

//TODO: Fix the assert keyword
//TODO: sanity check using assert, testing the language so far
//TODO: split(str, char) //how? Into an array?
//TODO: Format(string, args...) //array as an argument?

//TODO: IO library
//TODO: JSON library
//TODO: foreach(var element in object) //foreach(var char in "string")
//TODO: foreach(var index of object)
//TODO: switch-case

//TODO: import takes strings?
//TODO: in-game console
//TODO: assert as a "Standard" function?
//TODO: default arugments
//TODO: rest/spread operator; Min(...) Max(...)
//TODO: MAYBE initializer lists
//TODO: MAYBE destructuring
//TODO: MAYBE promises
//TODO: MAYBE coroutines
//TODO: MAYBE regex
//TODO: MAYBE template strings
//TODO: MAYBE namespaces
//TODO: MAYBE break x;
//TODO: MAYBE continue x;
//TODO: How would I do generics?
//TODO: automatic properties?

