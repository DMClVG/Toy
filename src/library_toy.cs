using System;
using CSString = System.String;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		public class Toy : IPlugin, IBundle {
			public override string ToString() { return "<native library>"; }

			//singleton pattern
			public IPlugin Singleton {
				get {
					if (singleton == null) {
						return singleton = new Toy();
					}
					return singleton;
				}
			}
			static Toy singleton = null;

			//version data
			double major = 0;
			double minor = 2;
			double patch = 5;

			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(CSString.IsNullOrEmpty(alias) ? "Toy" : alias, this, true);
			}

			//IBundle
			public object Property(Interpreter interpreter, Token token, object argument) {
				string propertyName = (string)argument;

				switch(propertyName) {
					case "Version": return $"{major}.{minor}.{patch}";
					case "Major": return major;
					case "Minor": return minor;
					case "Patch": return patch;

					case "VersionGreater": return new VersionGreater(this);
					case "VersionEqual": return new VersionEqual(this);
					case "VersionLess": return new VersionLess(this);

					case "Author": return "Kayne Ruse";

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}

			public class VersionGreater : ICallable {
				public override string ToString() { return "<native function>"; }

				Toy self;

				public VersionGreater(Toy self) {
					this.self = self;
				}

				public int Arity() {
					return 3;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					if (!(arguments[0] is double && arguments[1] is double && arguments[2] is double)) {
						throw new ErrorHandler.RuntimeError(token, "Version info must consist of numbers");
					}

					if ((double)arguments[0] > self.major) return false;
					if ((double)arguments[1] > self.minor) return false;
					if ((double)arguments[2] > self.patch) return false;
					return true;
				}
			}

			public class VersionEqual : ICallable {
				public override string ToString() { return "<native function>"; }

				Toy self;

				public VersionEqual(Toy self) {
					this.self = self;
				}

				public int Arity() {
					return 3;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					if (!(arguments[0] is double && arguments[1] is double && arguments[2] is double)) {
						throw new ErrorHandler.RuntimeError(token, "Version info must consist of numbers");
					}

					if ((double)arguments[0] != self.major) return false;
					if ((double)arguments[1] != self.minor) return false;
					if ((double)arguments[2] != self.patch) return false;
					return true;
				}
			}

			public class VersionLess : ICallable {
				public override string ToString() { return "<native function>"; }

				Toy self;

				public VersionLess(Toy self) {
					this.self = self;
				}

				public int Arity() {
					return 3;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					if (!(arguments[0] is double && arguments[1] is double && arguments[2] is double)) {
						throw new ErrorHandler.RuntimeError(token, "Version info must consist of numbers");
					}

					if ((double)arguments[0] < self.major) return false;
					if ((double)arguments[1] < self.minor) return false;
					if ((double)arguments[2] < self.patch) return false;
					return true;
				}
			}
		}
	}
}