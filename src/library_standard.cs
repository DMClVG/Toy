using System;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		class Standard : IPlugin {
			//singleton pattern
			public IPlugin Singleton {
				get {
					if (singleton == null) {
						return singleton = new Standard();
					}
					return singleton;
				}
			}
			static Standard singleton = null;

			//the persistent functors
			static Clock clock = new Clock();
			static Random random = new Random();
			static RandomSeed randomSeed = new RandomSeed();

			public void Initialize(Environment env, string alias) {
				if (String.IsNullOrEmpty(alias)) {
					//no alias, put these in the global scope
					env.Define("Clock", clock, true);
					env.Define("Random", random, true);
					env.Define("RandomSeed", randomSeed, true);
				} else {
					env.Define(alias, new Bundle(), true);
				}
			}

			//member class - the library as a bundle (for the alias)
			class Bundle : IBundle {
				public object Property(Interpreter interpreter, Token token, object argument) {
					string propertyName = (string)argument;

					switch(propertyName) {
						case "Clock": return clock;
						case "Random": return random;
						case "RandomSeed": return randomSeed;

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
				System.Random rand = null;

				public Random(int seed = int.MinValue) {
					if (seed == int.MinValue) {
						seed = new DateTimeOffset(DateTime.Now).Millisecond;
					}

					//initialize the randomizer
					if (rand == null) {
						rand = new System.Random(seed);
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

			class RandomSeed : ICallable {
				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					//TODO: hashing a string seed
					if (!(arguments[0] is double)) {
						throw new ErrorHandler.RuntimeError(token, "Unexpected type received (expected number)");
					}

					random = new Random((int)(double)arguments[0]);
					return null;
				}

				public override string ToString() { return "<native function>"; }
			}
		}
	}
}