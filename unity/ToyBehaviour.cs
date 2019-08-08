using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Toy {
	[RequireComponent(typeof(SpriteRenderer))]
	[RequireComponent(typeof(Rigidbody2D))]
	[RequireComponent(typeof(BoxCollider2D))]
	public class ToyBehaviour : MonoBehaviour, IBundle {
		//Toy members
		[SerializeField]
		string toyScript = "";

		Environment environment;
		public Dictionary<string, ScriptFunction> propertyMethods = new Dictionary<string, ScriptFunction>();

		ScriptFunction GetPropertyMethod(string propertyName) {
			if (!propertyMethods.ContainsKey(propertyName)) {
				//NO-OP
				return new ScriptFunction(new Function(new List<Expr>(), new List<Stmt>() { new Pass(new Token(TokenType.PASS, "", null, -1)) }), null);
			}
			return propertyMethods[propertyName];
		}

		//creation/destruction methods
		void Awake() {
			environment = Runner.RunFile("Assets/StreamingAssets/" + toyScript + ".toy");

			Runner.Run(environment, GetPropertyMethod("Awake"), new List<object>());
		}

		void Start() {
			Runner.Run(environment, GetPropertyMethod("Start"), new List<object>());
		}

		void OnDestroy() {
			Runner.Run(environment, GetPropertyMethod("OnDestroy"), new List<object>());
		}

		void OnEnable() {
			Runner.Run(environment, GetPropertyMethod("OnEnable"), new List<object>());
		}

		void OnDisable() {
			Runner.Run(environment, GetPropertyMethod("OnDisable"), new List<object>());
		}

		//loop methods
		void FixedUpdate() {
			Runner.Run(environment, GetPropertyMethod("FixedUpdate"), new List<object>());
		}

		void Update() {
			Runner.Run(environment, GetPropertyMethod("Update"), new List<object>());
		}

		void LateUpdate() {
			Runner.Run(environment, GetPropertyMethod("LateUpdate"), new List<object>());
		}

		//physics methods
		void OnCollisionEnter2D(Collision2D collision) {
			Runner.Run(environment, GetPropertyMethod("OnCollisionEnter2D"), new List<object>() { new GameObjectWrapper(collision.gameObject) });
		}

		void OnCollisionStay2D(Collision2D collision) {
			Runner.Run(environment, GetPropertyMethod("OnCollisionStay2D"), new List<object>() { new GameObjectWrapper(collision.gameObject) });
		}

		void OnCollisionExit2D(Collision2D collision) {
			Runner.Run(environment, GetPropertyMethod("OnCollisionExit2D"), new List<object>() { new GameObjectWrapper(collision.gameObject) });
		}

		void OnTriggerEnter2D(Collider2D collider) {
			Runner.Run(environment, GetPropertyMethod("OnTriggerEnter2D"), new List<object>() { new GameObjectWrapper(collider.gameObject) });
		}

		void OnTriggerStay2D(Collider2D collider) {
			Runner.Run(environment, GetPropertyMethod("OnTriggerStay2D"), new List<object>() { new GameObjectWrapper(collider.gameObject) });
		}

		void OnTriggerExit2D(Collider2D collider) {
			Runner.Run(environment, GetPropertyMethod("OnTriggerExit2D"), new List<object>() { new GameObjectWrapper(collider.gameObject) });
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

			public AssignableProperty(ToyBehaviour self, string propertyName) {
				this.self = self;
				this.propertyName = propertyName;
			}

			public override object Value {
				set {
					self.propertyMethods[propertyName] = (ScriptFunction)value;
				}
				get {
					return self.GetPropertyMethod(propertyName);
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

				case "OnCollisionEnter2D":
				case "OnCollisionStay2D":
				case "OnCollisionExit2D":

				case "OnTriggerEnter2D":
				case "OnTriggerStay2D":
				case "OnTriggerExit2D":

					return new AssignableProperty(this, propertyName);

				//game obeject references
				case "GameObject": return new GameObjectWrapper(gameObject);
				case "Rigidbody2D": return new Rigidbody2DWrapper(gameObject.GetComponent<Rigidbody2D>());
				case "ToyBehaviour": return gameObject.GetComponent<ToyBehaviour>();

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}
	}
}