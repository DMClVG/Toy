using System;
using System.Collections.Generic;
using UnityEngine;

namespace Toy {
	namespace Plugin {
		public class Unity : IPlugin, IBundle {
			//singleton pattern
			public IPlugin Singleton {
				get {
					if (singleton == null) {
						return singleton = new Unity();
					}
					return singleton;
				}
			}
			Unity singleton = null;

			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(String.IsNullOrEmpty(alias) ? "Unity" : alias, this, true);
			}

			//IBundle
			public object Property(Interpreter interpreter, Token token, object argument) {
				string propertyName = (string)argument;

				switch(propertyName) {
					case "Fetch": return new Fetch(this);

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}

			//callable types
			public class Fetch : ICallable {
				Unity self = null;

				public Fetch(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return GameObject.Find((string)arguments[0]).GetComponent<ToyBehaviour>();
				}
			}
		}
	}
}

