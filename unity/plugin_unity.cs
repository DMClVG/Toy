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
					case "FetchGameObject": return new FetchGameObject(this);
					case "LoadGameObject": return new LoadGameObject(this);
					case "LoadGameObjectAt": return new LoadGameObjectAt(this);

					//virtual input members
					case "GetAxis": return new GetAxis(this);
					case "GetButton": return new GetButton(this);
					case "GetButtonDown": return new GetButtonDown(this);
					case "GetButtonUp": return new GetButtonUp(this);

					//time members
					case "Time": return (double)Time.time;
					case "DeltaTime": return (double)Time.deltaTime;

					case "FixedTime": return (double)Time.fixedTime;
					case "FixedDeltaTime": return (double)Time.fixedDeltaTime;

					case "UnscaledTime": return (double)Time.unscaledTime;
					case "UnscaledDeltaTime": return (double)Time.unscaledDeltaTime;

					case "FixedUnscaledTime": return (double)Time.fixedUnscaledTime;
					case "FixedUnscaledDeltaTime": return (double)Time.fixedUnscaledDeltaTime;

					case "TimeScale": return new AssignableProperty((val) => Time.timeScale = (float)(double)val, (x) => (double)Time.timeScale);

					//TODO: real input members

					//TODO: UI

					//TODO: get child

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}

			//assignable properties
			public class AssignableProperty : AssignableIndex {
				Func<object, object> Set = null;
				Func<object, object> Get = null;

				public AssignableProperty(Func<object, object> Set, Func<object, object> Get) {
					this.Set = Set;
					this.Get = Get;
				}

				public override object Value {
					set {
						Set(value);
					}
					get {
						return Get(null);
					}
				}
			}

			//access members
			public class FetchGameObject : ICallable {
				Unity self = null;

				public FetchGameObject(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return new GameObjectWrapper(GameObject.Find((string)arguments[0]));
				}
			}

			public class LoadGameObject : ICallable {
				Unity self = null;

				public LoadGameObject(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return new GameObjectWrapper(GameObject.Instantiate(Resources.Load((string)arguments[0]) as GameObject));
				}
			}

			public class LoadGameObjectAt : ICallable {
				Unity self = null;

				public LoadGameObjectAt(Unity self) {
					this.self = self;
				}

				public int Arity() {
					return 1 + 3 + 3;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					Vector3 position;
					position.x = (float)(double)arguments[1];
					position.y = (float)(double)arguments[2];
					position.z = (float)(double)arguments[3];

					Vector3 rotation;
					rotation.x = (float)(double)arguments[4];
					rotation.y = (float)(double)arguments[5];
					rotation.z = (float)(double)arguments[6];

					return new GameObjectWrapper(GameObject.Instantiate(Resources.Load((string)arguments[0]) as GameObject, position, Quaternion.Euler(rotation.x, rotation.y, rotation.z)));
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

