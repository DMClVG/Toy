using System.Collections.Generic;

namespace Toy {
	//for any class that maps to a library
	interface IPlugin {
		//this is called when the script calls: import "plugin"
		//inside, the environment object can be loaded with objects deriving
		//from the interfaces below
		//the whole plugin should be renamed to the value of alias if it is not
		//null
		void Initialize(Environment env, string alias);
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
		//first is always defined
		//if end is provided, second will also be defined
		//if step is provided, third will also be defined
		//if first has no value, it will be double.NegativeInfinity
		//if second has no value, it will be double.PositiveInfinity
		//if third is defined, it will always have a value
		//the return is returned in the script
		object Access(Interpreter interpreter, Token token, object first, object second, object third);
	}

	//for any class that maps to a type
	interface IBundle {
		//called when a property is accessed
		//the argument is the property name
		//the return is returned in the script
		object Property(Interpreter interpreter, Token token, object argument);
	}

	//helper class (for assigning to indexed values)
	abstract class AssignableIndex {
		public abstract object Value {
			get;
			set;
		}
	}
}