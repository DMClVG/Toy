using System;
using System.Collections.Generic;

namespace Toy {
	class Environment {
		//"bool" in the tuple is true if it's a constant that can't be changed
		Dictionary<string, Tuple<bool, object>> values = new Dictionary<string, Tuple<bool, object>>();

		public void Define(Token name, object value, bool constant) {
			if (values.ContainsKey(name.lexeme)) {
				throw new ErrorHandler.RuntimeError(name, "Can't redefine a variable");
			}

			values[name.lexeme] = new Tuple<bool, object>(constant, value);
		}

		public object Get(Token name) {
			if (!values.ContainsKey(name.lexeme)) {
				return null; //all undefined variables are null
			}

			return values[name.lexeme].Item2;
		}

		public object Set(Token name, object value) {
			if (!values.ContainsKey(name.lexeme)) {
				throw new ErrorHandler.RuntimeError(name, "Undefined variable '" + name.lexeme + "'");
			}

			if (values[name.lexeme].Item1) {
				throw new ErrorHandler.RuntimeError(name, "Can't assign to a constant");
			}

			return values[name.lexeme] = new Tuple<bool, object>(false, value);
		}
	}
}