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

			switch(propertyName) {
				//simple members
				case "name": return self.name;
				case "tag": return self.tag;

				//game obeject references
				case "GameObject": return new GameObjectWrapper(self);
				case "Rigidbody2D": return new Rigidbody2DWrapper(self.GetComponent<Rigidbody2D>());
				case "ToyBehaviour": return self.GetComponent<ToyBehaviour>();

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}
	}
}