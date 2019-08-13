using System;
using System.Collections.Generic;
using UnityEngine;

namespace Toy {
	class GameObjectWrapper : IBundle {
		GameObject self = null;

		public GameObjectWrapper(GameObject self) {
			this.self = self;
		}

		//IBundle
		public object Property(Interpreter interpreter, Token token, object argument) {
			string propertyName = (string)argument;

			if (self == null) {
				throw new ErrorHandler.RuntimeError(token, "Can't access property of null Unity GameObject");
			}

			switch(propertyName) {
				//simple members
				case "name": return self.name;
				case "tag": return self.tag;

				//control members
				case "Destroy": return new DestroyCallable(this);

				//game obeject components
				case "Behaviour": return self.GetComponent<ToyBehaviour>();
				case "Transform": return new TransformWrapper(self.GetComponent<Transform>());
				case "Rigidbody2D": return new Rigidbody2DWrapper(self.GetComponent<Rigidbody2D>());

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}

		public class DestroyCallable : ICallable {
			GameObjectWrapper self = null;

			public DestroyCallable(GameObjectWrapper self) {
				this.self = self;
			}

			public int Arity() {
				return 0;
			}

			public object Call(Interpreter interpreter, Token token, List<object> arguments) {
				UnityEngine.Object.Destroy(self.self);
				self.self = null;
				return null;
			}

			public override string ToString() { return "<Unity function>"; }
		}

		public override string ToString() { return "<Unity GameObject>"; }
	}
}