using System;

namespace Toy {
	namespace Library {
		class Toy : IPlugin, IBundle {
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

			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(String.IsNullOrEmpty(alias) ? "Toy" : alias, this, true);
			}

			//IBundle
			public object Property(Interpreter interpreter, Token token, object argument) {
				string propertyName = (String)argument;

				switch(propertyName) {
					case "version": return 0.1;
					case "author": return "Kayne Ruse";

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}
		}
	}
}