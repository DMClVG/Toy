using System;
using List = System.Collections.Generic.List<object>;
using Dict = System.Collections.Generic.Dictionary<object, object>;
using Pair = System.Collections.Generic.KeyValuePair<object, object>;

namespace Toy {
	namespace Plugin {
		//the plugin class
		class Dictionary : IPlugin, ICallable {
			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(String.IsNullOrEmpty(alias) ? "Dictionary" : alias, this, true);
			}

			//ICallable
			public int Arity() {
				return 0;
			}

			public object Call(Interpreter Interpreter, Token token, List arguments) {
				return new DictionaryInstance(new Dict());
			}

			//the index assign helper class
			class DictionaryAssignableIndex : AssignableIndex {
				Dict container;
				object index;

				public DictionaryAssignableIndex(Dict container, object index) {
					this.container = container;
					this.index = index;
				}

				public override object Value {
					get {
						return this.container.ContainsKey(index) ? this.container[this.index] : null;
					}
					set {
						if (this.container.ContainsKey(this.index)) {
							this.container.Remove(this.index);
						}
						this.container.Add(this.index, value);
					}
				}
			}

			//the instance class
			class DictionaryInstance : ICollection, IBundle {
				//container members
				Dict container = null;

				//methods
				public DictionaryInstance(Dict dict) {
					container = dict;
				}

				//ICollection
				public object Access(Interpreter interpreter, Token token, object first, object second, object third) {
					if (first is double && (double)first == double.NegativeInfinity && second is double && (double)second == double.PositiveInfinity) {
						return new DictionaryInstance(new Dict(this.container));
					} else if (second != null || third != null) {
						throw new ErrorHandler.RuntimeError(token, "Can't use the slice notation with a Dictionary, except Dictionary[:]");
					}

					return new DictionaryAssignableIndex(container, first);
				}

				public object Property(Interpreter interpreter, Token token, object argument) {
					string propertyName = (string)argument;

					switch(propertyName) {
						case "Insert": return new Insert(this);
						case "Delete": return new Delete(this);
						case "Length": return new Length(this);
						case "Contains": return new Contains(this);
						case "ToString": return new ToStringCallable(this);

						default:
							throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
					}
				}

				class Insert : ICallable {
					DictionaryInstance self = null;

					public Insert(DictionaryInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 2;
					}

					public object Call(Interpreter interpreter, Token token, List arguments) {
						self.container[arguments[0]] = arguments[1];
						return null;
					}

					public override string ToString() { return "<Dictionary function>"; }
				}

				class Delete : ICallable {
					DictionaryInstance self = null;

					public Delete(DictionaryInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 1;
					}

					public object Call(Interpreter interpreter, Token token, List arguments) {
						self.container.Remove(arguments[0]);
						return null;
					}

					public override string ToString() { return "<Dictionary function>"; }
				}

				class Length : ICallable {
					DictionaryInstance self = null;

					public Length(DictionaryInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 0;
					}

					public object Call(Interpreter interpreter, Token token, List arguments) {
						return self.container.Count;
					}

					public override string ToString() { return "<Dictionary function>"; }
				}

				class Contains : ICallable {
					DictionaryInstance self = null;

					public Contains(DictionaryInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 1;
					}

					public object Call(Interpreter interpreter, Token token, List arguments) {
						return self.container.ContainsKey(arguments[0]);
					}

					public override string ToString() { return "<Dictionary function>"; }
				}

				class ToStringCallable : ICallable {
					DictionaryInstance self = null;

					public ToStringCallable(DictionaryInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 0;
					}

					public object Call(Interpreter interpreter, Token token, List arguments) {
						return self.ToString();
					}

					public override string ToString() { return "<Dictionary function>"; }
				}

				public override string ToString() {
					//prevent circular references
					if (ToStringHelper.passed.ContainsKey(this)) {
						return "<circular reference>";
					}
					ToStringHelper.passed[this] = 0;

					//build the result
					string result = "";
					foreach(Pair kvp in container) {
						result += kvp.Key.ToString() + ":";
						result += kvp.Value.ToString() + ",";
					}

					//trim the last ',' character
					if (result.Length > 0 && result[result.Length - 1] == ',') {
						result = result.Substring(0, result.Length - 1);
					}

					//cleanup and return
					ToStringHelper.passed.Remove(this);
					return "{" + result + "}";
				}
			}

			public override string ToString() { return "<plugin>"; }
		}
	}
}