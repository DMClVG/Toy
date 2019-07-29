using System;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		class Standard : IPlugin {
			public void Initialize(Environment env) {
				env.Define("Clock", new Clock(), true);
			}

			//member classes representing functions
			class Clock : ICallable {
				public int Arity() {
					return 0;
				}

				public object Call(Interpreter interpreter, List<object> arguments) {
					return new DateTimeOffset(DateTime.Now).ToUnixTimeMilliseconds() / (double)1000;
				}

				public override string ToString() { return "<native function>"; }
			}
		}
	}
}