using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Toy {
	public class ToyBehaviour : MonoBehaviour, IBundle {
		//Toy members
		[SerializeField]
		string toyScript = "";

		Environment environment;
		public Dictionary<string, ScriptFunction> propertyMethods = new Dictionary<string, ScriptFunction>();

		ScriptFunction GetPropertyMethod(string propertyName, int argCount) {
			if (!propertyMethods.ContainsKey(propertyName)) {
				//dummy args
				List<Expr> args = new List<Expr>();
				for (int i = 0; i < argCount; i++) {
					args.Add(new Variable( new Token(TokenType.IDENTIFIER, "param" + i.ToString(), null, -1) ));
				}

				//NO-OP
				return new ScriptFunction(new Function(args, new List<Stmt>() { new Pass(new Token(TokenType.PASS, "", null, -1)) }), null);
			}
			return propertyMethods[propertyName];
		}

		//creation/destruction methods
		void Awake() {
			environment = Runner.RunFile("Assets/StreamingAssets/" + toyScript + ".toy");

			Runner.Run(environment, GetPropertyMethod("Awake", 0), new List<object>());
		}

		void Start() {
			Runner.Run(environment, GetPropertyMethod("Start", 0), new List<object>());
		}

		void OnDestroy() {
			Runner.Run(environment, GetPropertyMethod("OnDestroy", 0), new List<object>());
		}

		void OnEnable() {
			Runner.Run(environment, GetPropertyMethod("OnEnable", 0), new List<object>());
		}

		void OnDisable() {
			Runner.Run(environment, GetPropertyMethod("OnDisable", 0), new List<object>());
		}

		//loop methods
		void FixedUpdate() {
			Runner.Run(environment, GetPropertyMethod("FixedUpdate", 0), new List<object>());
		}

		void Update() {
			Runner.Run(environment, GetPropertyMethod("Update", 0), new List<object>());
		}

		void LateUpdate() {
			Runner.Run(environment, GetPropertyMethod("LateUpdate", 0), new List<object>());
		}

		//physics methods
		void OnCollisionEnter2D(Collision2D collision) {
			Runner.Run(environment, GetPropertyMethod("OnCollisionEnter2D", 1), new List<object>() { new GameObjectWrapper(collision.gameObject) });
		}

		void OnCollisionStay2D(Collision2D collision) {
			Runner.Run(environment, GetPropertyMethod("OnCollisionStay2D", 1), new List<object>() { new GameObjectWrapper(collision.gameObject) });
		}

		void OnCollisionExit2D(Collision2D collision) {
			Runner.Run(environment, GetPropertyMethod("OnCollisionExit2D", 1), new List<object>() { new GameObjectWrapper(collision.gameObject) });
		}

		void OnTriggerEnter2D(Collider2D collider) {
			Runner.Run(environment, GetPropertyMethod("OnTriggerEnter2D", 1), new List<object>() { new GameObjectWrapper(collider.gameObject) });
		}

		void OnTriggerStay2D(Collider2D collider) {
			Runner.Run(environment, GetPropertyMethod("OnTriggerStay2D", 1), new List<object>() { new GameObjectWrapper(collider.gameObject) });
		}

		void OnTriggerExit2D(Collider2D collider) {
			Runner.Run(environment, GetPropertyMethod("OnTriggerExit2D", 1), new List<object>() { new GameObjectWrapper(collider.gameObject) });
		}

		//input methods
		void OnMouseEnter() {
			//TODO
		}

		void OnMouseOver() {
			//TODO
		}

		void OnMouseExit() {
			//TODO
		}

		//assignable properties
		public class AssignableProperty : AssignableIndex {
			ToyBehaviour self = null;
			string propertyName;
			int argCount = -1;

			public AssignableProperty(ToyBehaviour self, string propertyName, int argCount) {
				this.self = self;
				this.propertyName = propertyName;
				this.argCount = argCount;
			}

			public override object Value {
				set {
					self.propertyMethods[propertyName] = (ScriptFunction)value;
				}
				get {
					return self.GetPropertyMethod(propertyName, argCount);
				}
			}
		}

		//IBundle
		public object Property(Interpreter interpreter, Token token, object argument) {
			string propertyName = (string)argument;

			switch(propertyName) {
				case "Awake":
				case "Start":
				case "OnDestroy":
				case "OnEnable":
				case "OnDisable":
				case "FixedUpdate":
				case "Update":
				case "LateUpdate":

				return new AssignableProperty(this, propertyName, 0);

				case "OnCollisionEnter2D":
				case "OnCollisionStay2D":
				case "OnCollisionExit2D":

				case "OnTriggerEnter2D":
				case "OnTriggerStay2D":
				case "OnTriggerExit2D":

					return new AssignableProperty(this, propertyName, 1);

				//game obeject references
				case "GameObject": return new GameObjectWrapper(gameObject);
				case "Rigidbody2D": return new Rigidbody2DWrapper(gameObject.GetComponent<Rigidbody2D>());

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}

		public override string ToString() { return "<Unity ToyBehaviour>"; }
	}
}