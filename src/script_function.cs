using System.Collections.Generic;

namespace Toy {
	public class ScriptFunction : ICallable {
		Function declaration;
		Environment closure;

		public ScriptFunction(Function func, Environment env) {
			declaration = func;
			closure = env;
		}

		public int Arity() {
			return declaration.parameters.Count;
		}

		public object Call(Interpreter interpreter, Token token, List<object> arguments) {
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

		//build an empty function, 0 parameters, PASS statement (for use in unity)
		public static ScriptFunction NOOP = new ScriptFunction(new Function(new List<Expr>(), new List<Stmt>() { new Pass(new Token(TokenType.PASS, "", null, -1)) }), null);
	}
}