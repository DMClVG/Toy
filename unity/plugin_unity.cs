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
					//access members
					case "Fetch": return new Fetch(this);
					case "Load": return new Load(this);

					//virtual input members
					case "GetAxis": return new GetAxis(this);
					case "GetButton": return new GetButton(this);
					case "GetButtonDown": return new GetButtonDown(this);
					case "GetButtonUp": return new GetButtonUp(this);

					//TODO: real input members

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}

			//access members
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

			public class Load : ICallable {
				Unity self = null;

				public Load(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return new GameObjectWrapper(GameObject.Instantiate(Resources.Load((string)arguments[0]) as GameObject));
				}
			}

			//virtual input members
			public class GetAxis : ICallable {
				Unity self = null;

				public GetAxis(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return Input.GetAxis((string)arguments[0]);
				}

				public override string ToString() { return "<Unity function>"; }
			}

			public class GetButton : ICallable {
				Unity self = null;

				public GetButton(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return Input.GetButton((string)arguments[0]);
				}

				public override string ToString() { return "<Unity function>"; }
			}

			public class GetButtonDown : ICallable {
				Unity self = null;

				public GetButtonDown(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return Input.GetButtonDown((string)arguments[0]);
				}

				public override string ToString() { return "<Unity function>"; }
			}

			public class GetButtonUp : ICallable {
				Unity self = null;

				public GetButtonUp(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return Input.GetButtonUp((string)arguments[0]);
				}

				public override string ToString() { return "<Unity function>"; }
			}

			public override string ToString() { return "<Unity plugin>"; }
		}
	}
}

