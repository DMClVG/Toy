using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

namespace Toy {
	namespace PluginExtras {
	public class TextMeshWrapper : IBundle {
		public override string ToString() { return "<Unity TextMesh wrapper>"; }

		TextMeshProUGUI self = null;

		public TextMeshWrapper(TextMeshProUGUI self) {
			this.self = self;
		}

		//IBundle
		public object Property(Interpreter interpreter, Token token, object argument) {
			string propertyName = (string)argument;

			switch(propertyName) {
				case "SetText": return new SetText(this);
				case "GetText": return new GetText(this);

				//game obeject references
				case "GameObject": return new GameObjectWrapper(self.gameObject);

				default:
					throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
			}
		}

		//callables
		class GetText : ICallable {
			public override string ToString() { return "<Unity function>"; }

			TextMeshWrapper self = null;

			public GetText(TextMeshWrapper self) {
				this.self = self;
			}

				public int Arity() {
					return 0;
			}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return self.self.text;
				}
			}

			class SetText : ICallable {
				public override string ToString() { return "<Unity function>"; }

				TextMeshWrapper self = null;

				public SetText(TextMeshWrapper self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					return self.self.text = (string)arguments[0];
				}
			}
		}
	}
}
