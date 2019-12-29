using CSMath = System.Math;
using CSString = System.String;
using System;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		class IO : IPlugin, IBundle {
			public override string ToString() { return "<native library>"; }

			//singleton pattern
			public IPlugin Singleton {
				get {
					if (singleton == null) {
						return singleton = new IO();
					}
					return singleton;
				}
			}
			static IO singleton = null;

			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(CSString.IsNullOrEmpty(alias) ? "IO" : alias, this, true);
			}

			//IBundle
			public object Property(Interpreter interpreter, Token token, object argument) {
				string propertyName = (string)argument;

				switch(propertyName) {
					case "ReadLine": return new ReadLine();
					case "ReadChar": return new ReadChar();
					case "ReadNumber": return new ReadNumber();
					case "Write": return new Write();

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}

			//member classes representing functions
			public class ReadLine : ICallable {
				public override string ToString() { return "<native function>"; }

				public int Arity() {
					return 0;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return Console.ReadLine();
				}
			}

			public class ReadChar : ICallable {
				public override string ToString() { return "<native function>"; }

				public int Arity() {
					return 0;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return Console.ReadKey().KeyChar.ToString();
				}
			}

			public class ReadNumber : ICallable {
				public override string ToString() { return "<native function>"; }

				public int Arity() {
					return 0;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string input = Console.ReadLine();

					try {
						return double.Parse(input);
					} catch (Exception) {
						return null;
					}
				}
			}

			public class Write : ICallable {
				public override string ToString() { return "<native function>"; }

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					//kind of copied from the print keyword
					if (arguments[0] is string) {
						Console.Write(System.Text.RegularExpressions.Regex.Unescape((string)arguments[0]));
					} else if (arguments[0] is bool) {
						Console.Write(arguments[0].ToString().ToLower());
					} else if (arguments[0] == null) {
						Console.Write("null");
					} else {
						Console.Write(arguments[0]);
					}

					return null;
				}
			}
		}
	}
}