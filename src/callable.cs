using System.Collections.Generic;

namespace Toy {
	abstract class Callable {
		public abstract int Arity();
		public abstract object Call(Interpreter interpreter, List<object> arguments);
	}
}