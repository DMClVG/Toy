using System.Collections.Generic;

namespace Toy {
	abstract class Callable {
		public abstract int Arity();
		public abstract object Call(Interpreter interpreter, List<object> arguments);
	}

	class ScriptFunction : Callable {
		Function declaration;
		Environment closure;

		public ScriptFunction(Function func, Environment env) {
			declaration = func;
			closure = env;
		}

		public override int Arity() { return declaration.parameters.Count; }
		public override object Call(Interpreter interpreter, List<object> arguments) {
			Environment environment = new Environment(closure);

			for (int i = 0; i < declaration.parameters.Count; i++) {
				environment.Define(((Variable)declaration.parameters[i]).name.lexeme, arguments[i], false);
			}

			try {
				interpreter.ExecuteBlock(new Block(declaration.body, false), environment, true);
			} catch(ReturnException e) {
				return e.result;
			}

			return null;
		}
		public override string ToString() { return "<function>"; }
	}
}