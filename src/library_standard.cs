using System;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		class Standard : IPlugin {
			public void Initialize(Environment env) {
				env.Define("Clock", new Clock(), true);
				env.Define("Random", new Random(), true);
			}

			//member classes representing functions
			class Clock : ICallable {
				public int Arity() {
					return 0;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return new DateTimeOffset(DateTime.Now).ToUnixTimeMilliseconds() / (double)1000;
				}

				public override string ToString() { return "<native function>"; }
			}

			class Random : ICallable {
				static System.Random rand = null;

				public Random() {
					//initialize the randomizer
					if (rand == null) {
						rand = new System.Random((new DateTimeOffset(DateTime.Now).Millisecond));
					}
				}

				public int Arity() {
					return 0;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return rand.NextDouble();
				}

				public override string ToString() { return "<native function>"; }
			}
		}
	}
}