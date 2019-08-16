using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

namespace Toy {
	public class TextMeshWrapper : IBundle {
		TextMeshProUGUI self = null;

		public TextMeshWrapper(TextMeshProUGUI self) {
			this.self = self;
		}

		//IBundle
		public object Property(Interpreter interpreter, Token token, object argument) {
			string propertyName = (string)argument;

			switch(propertyName) {
				case "Interface": return self.GetComponent<ToyInterface>();

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}

		public override string ToString() { return "<Unity TextMesh>"; }
	}
}
