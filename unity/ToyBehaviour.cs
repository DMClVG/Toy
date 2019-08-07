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

		//creation/destruction methods
		void Awake() {
			environment = Runner.RunFile("Assets/StreamingAssets/" + toyScript + ".toy");
			Runner.Run(environment, $"const __instance = Unity.Fetch(\"{this.gameObject.name}\");");
			Runner.Run(environment, "__instance.Awake();");
		}

		void Start() {
			Runner.Run(environment, "__instance.Start();");
		}

		void OnDestroy() {
			Runner.Run(environment, "__instance.OnDestroy();");
		}

		void OnEnable() {
			Runner.Run(environment, "__instance.OnEnable();");
		}

		void OnDisable() {
			Runner.Run(environment, "__instance.OnDisable();");
		}

		//loop methods
		void FixedUpdate() {
			Runner.Run(environment, "__instance.FixedUpdate();");
		}

		void Update() {
			Runner.Run(environment, "__instance.Update();");
		}

		void LateUpdate() {
			Runner.Run(environment, "__instance.LateUpdate();");
		}

		//physics methods
		void OnTriggerEnter2D(Collider2D collider) {
			//TODO
		}

		void OnTriggerStay2D(Collider2D collider) {
			//TODO
		}

		void OnTriggerExit2D(Collider2D collider) {
			//TODO
		}

		void OnCollisionEnter2D(Collision2D collision) {
			//TODO
		}

		void OnCollisionStay2D(Collision2D collision) {
			//TODO
		}

		void OnCollisionExit2D(Collision2D collision) {
			//TODO
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
					if (!self.propertyMethods.ContainsKey(propertyName)) {
						return ScriptFunction.NOOP;
					}
					return self.propertyMethods[propertyName];
				}
			}
		}

		//IBundle
		public object Property(Interpreter interpreter, Token token, object argument) {
			string propertyName = (string)argument;

			switch(propertyName) {
				case "Rigidbody2D":
					return new RigidbodyWrapper(GetComponent<Rigidbody2D>());

				case "Awake":
				case "Start":
				case "OnDestroy":
				case "OnEnable":
				case "OnDisable":
				case "FixedUpdate":
				case "Update":
				case "LateUpdate":
					//TODO: more
					return new AssignableProperty(this, propertyName);

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}
	}
}