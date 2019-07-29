using System;
using System.Collections.Generic;

using static Toy.TokenType;

namespace Toy {
	class Environment {
		//members
		Environment enclosing;

		//"bool" in the tuple is true if it's a constant that can't be changed
		Dictionary<string, Tuple<bool, object>> values = new Dictionary<string, Tuple<bool, object>>();

		//methods
		public Environment() {
			enclosing = null;
		}

		public Environment(Environment other) {
			enclosing = other;
		}

		public void Define(string name, object value, bool constant) {
			Define(new Token(EOF, name, null, -1), value, constant);
		}

		public void Define(Token name, object value, bool constant) {
			if (values.ContainsKey(name.lexeme)) {
				throw new ErrorHandler.RuntimeError(name, "Can't redefine a variable");
			}

			values[name.lexeme] = new Tuple<bool, object>(constant, value);
		}

		public object Get(Token name) {
			if (!values.ContainsKey(name.lexeme)) {
				if (enclosing == null) {
					return null; //all undefined variables are null
				} else {
					return enclosing.Get(name);
				}
			}

			return values[name.lexeme].Item2;
		}

		public object GetAt(int distance, Token name) {
			return Ancestor(distance).Get(name);
		}

		public object Set(Token name, object value) {
			if (!values.ContainsKey(name.lexeme)) {
				if (enclosing == null) {
					throw new ErrorHandler.RuntimeError(name, "Undefined variable '" + name.lexeme + "'");
				} else {
					return enclosing.Set(name, value);
				}
			}

			if (values[name.lexeme].Item1) {
				throw new ErrorHandler.RuntimeError(name, "Can't assign to a constant");
			}

			return values[name.lexeme] = new Tuple<bool, object>(false, value);
		}

		public object SetAt(int distance, Token name, object value) {
			return Ancestor(distance).Set(name, value);
		}

		Environment Ancestor(int distance) {
			Environment env = this;
			for (int i = 0; i < distance; i++) {
				env = env.enclosing;
			}
			return env;
		}
	}
}