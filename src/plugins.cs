using System.Collections.Generic;

namespace Toy {
	//for any class that maps to a library
	interface IPlugin {
		//this is called when the script calls: import "plugin"
		//inside, the environment object can be loaded with objects deriving
		//from the interfaces below
		void Initialize(Environment env);
	}

	//for any class that maps to a Toy function
	interface ICallable {
		//the number of arguments it takes
		int Arity();

		//the call function
		//the return is returned in the script
		object Call(Interpreter interpreter, Token token, List<object> arguments);
	}

	//for any class that maps to bracket access
	interface ICollection {
		//called when the bracket syntax is used
		//arguments are in the format [first:second:third]
		//these normally represent "beginning", "end" and "step"
		//Any that aren't proveded will be null, starting from the end
		//the return is returned in the script
		object Access(Interpreter interpreter, Token token, List<object> arguments);
	}

	//for any class that maps to a type
	interface IBundle {
		//called when a property is accessed
		//each string argument is another link in the chain of properties
		//object.property1.property2.property3...
		//the return is returned in the script
		object Property(Interpreter interpreter, Token token, List<object> arguments);
	}
}