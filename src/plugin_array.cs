using System;
using System.Collections.Generic;

namespace Toy {
	namespace Plugin {
		//the plugin class
		class Array : IPlugin, ICallable {
			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(String.IsNullOrEmpty(alias) ? "Array" : alias, this, true);
			}

			//ICallable
			public int Arity() {
				return 0;
			}

			public object Call(Interpreter Interpreter, Token token, List<object> arguments) {
				return new ArrayInstance(arguments);
			}

			//the instance class
			class ArrayInstance : ICollection, IBundle {
				//container members
				public List<object> memberList = new List<object>();

				//methods
				public ArrayInstance(List<object> arguments) {
					//DO NOTHING
				}

				//ICollection
				public object Access(Interpreter interpreter, Token token, List<object> arguments) {
					//TODO
					return null;
				}

				//IBundle
				public object Property(Interpreter interpreter, Token token, object argument) {
					string propertyName = (string)argument;

					switch(propertyName) {
						case "Push": return new Push(this);
						case "Pop": return new Pop(this);
						case "Unshift": return new Unshift(this);
						case "Shift": return new Shift(this);
						case "Length": return new Length(this);
						case "Sort": return new Sort(this);
						case "Insert": return new Insert(this);
						case "Delete": return new Delete(this);
						case "ToString": return new ToStringCallable(this);

						default:
							throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
					}
				}

				class Push : ICallable {
					ArrayInstance self = null;

					public Push(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 1;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						self.memberList.Add(arguments[0]);
						return null;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Pop : ICallable {
					ArrayInstance self = null;

					public Pop(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 0;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						object result = self.memberList[self.memberList.Count - 1];
						self.memberList.RemoveAt(self.memberList.Count - 1);
						return result;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Unshift : ICallable {
					ArrayInstance self = null;

					public Unshift(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 1;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						self.memberList.Insert(0, arguments[0]);
						return null;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Shift : ICallable {
					ArrayInstance self = null;

					public Shift(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 0;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						object result = self.memberList[0];
						self.memberList.RemoveAt(0);
						return result;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Length : ICallable {
					ArrayInstance self = null;

					public Length(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 0;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						return self.memberList.Count;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Sort : ICallable {
					ArrayInstance self = null;
					ScriptFunction comparator = null;
					Interpreter interpreter = null;
					Token token = null;

					public Sort(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 1;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						this.comparator = (ScriptFunction)arguments[0];
						this.interpreter = interpreter;
						this.token = token;

						self.memberList.Sort(SortComparison);

						return null;
					}

					int SortComparison(object lhs, object rhs) {
						object result = comparator.Call(interpreter, token, new List<object>() { lhs, rhs });
						if (!(result is double)) {
							throw new ErrorHandler.RuntimeError(token, "Unexpected result type from comparator function (expected number)");
						}

						return Convert.ToInt32(result);
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Insert : ICallable {
					ArrayInstance self = null;

					public Insert(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 2;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						self.memberList.Insert(Convert.ToInt32(arguments[0]), arguments[1]);
						return null;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class Delete : ICallable {
					ArrayInstance self = null;

					public Delete(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 1;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						self.memberList.RemoveAt(Convert.ToInt32(arguments[0]));
						return null;
					}

					public override string ToString() { return "<Array function>"; }
				}

				class ToStringCallable : ICallable {
					ArrayInstance self = null;

					public ToStringCallable(ArrayInstance self) {
						this.self = self;
					}

					public int Arity() {
						return 0;
					}

					public object Call(Interpreter interpreter, Token token, List<object> arguments) {
						return self.ToString();
					}

					public override string ToString() { return "<Array function>"; }
				}

				public override string ToString() {
					string result = "";

					foreach(object o in memberList) {
						if (o is Literal) {
							result += ((Literal)o).ToString();
						} else {
							result += o.ToString();
						}

						result += ",";
					}

					if (result.Length > 0 && result[result.Length - 1] == ',') {
						result = result.Substring(0, result.Length - 1);
					}

					return "[" + result + "]";
				}
			}

			public override string ToString() { return "<plugin>"; }
		}
	}
}