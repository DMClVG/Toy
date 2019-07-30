using System;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		class Standard : IPlugin {
			public void Initialize(Environment env, string alias) {
				if (String.IsNullOrEmpty(alias)) {
					//no alias, put these in the global scope
					env.Define("Clock", new Clock(), true);
					env.Define("Random", new Random(), true);
				} else {
					env.Define(alias, new Bundle(), true);
				}
			}

			//member class - the library as a bundle (for the alias)
			class Bundle : IBundle {
				Clock clock = new Clock();
				Random random = new Random();

				public object Property(Interpreter interpreter, Token token, object argument) {
					string propertyName = (string)argument;

					switch(propertyName) {
						case "Clock": return clock;
						case "Random": return random;

						default:
							throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
					}
				}
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