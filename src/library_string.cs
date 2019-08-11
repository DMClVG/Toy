using CSString = System.String;
using System.Collections.Generic;

namespace Toy {
	namespace Library {
		class String : IPlugin, IBundle {
			//singleton pattern
			public IPlugin Singleton {
				get {
					if (singleton == null) {
						return singleton = new String();
					}
					return singleton;
				}
			}
			static String singleton = null;

			//IPlugin
			public void Initialize(Environment env, string alias) {
				env.Define(CSString.IsNullOrEmpty(alias) ? "String" : alias, this, true);
			}

			//static members
			public static object SliceNotationLiteral(Expr callee, Token token, object first, object second, object third) {
				//bounds checking
				if (!(first is double) || ((double)first != double.NegativeInfinity && (double)first < 0) || ((double)first != double.NegativeInfinity && (double)first >= ((string)((Literal)callee).value).Length)) {
					throw new ErrorHandler.RuntimeError(token, "First index must be a number and between 0 and String.Length(string) -1 (inclusive)");
				}

				if (second != null) {
					if (!(second is double) || ((double)second != double.PositiveInfinity && (double)second < 0) || ((double)second != double.PositiveInfinity && (double)second >= ((string)((Literal)callee).value).Length)) {
						throw new ErrorHandler.RuntimeError(token, "Second index must be a number and between 0 and String.Length(string) -1 (inclusive)");
					}
				}

				if (second == null) {
					//access this character only
					return ((string)((Literal)callee).value)[(int)(double)first];
				}

				//default values for slice notation (begin and end are inclusive)
				int begin = (double)first == double.NegativeInfinity ? 0 : (int)(double)first;
				int end = (double)second == double.PositiveInfinity ? ((string)((Literal)callee).value).Length - 1 : (int)(double)second;
				int step = third == null ? 1 : (int)(double)third;

				//check for infinite loops
				if (step == 0) {
					throw new ErrorHandler.RuntimeError(token, "Can't have a string step of 0");
				}

				//build the new string
				string newString = "";
				for (int index = step > 0 ? begin : end; index >= begin && index <= end; index += step) {
					newString += ((string)((Literal)callee).value)[index];
				}

				return new StringLiteralAssignableIndex(callee, newString, begin, end, token, third);
			}

			public class StringLiteralAssignableIndex : AssignableIndex {
				Expr callee;
				string newString;
				int begin;
				int end;
				Token token;
				object third;

				public StringLiteralAssignableIndex(Expr callee, string newString, int begin, int end, Token token, object third) {
					this.callee = callee;
					this.newString = newString;
					this.begin = begin;
					this.end = end;
					this.token = token;
					this.third = third;
				}

				public override object Value {
					get {
						return newString;
					}
					set {
						//disallow a third index
						if (third != null) {
							throw new ErrorHandler.RuntimeError(token, "Unexpected third slice index found");
						}

						//switch
						if (begin > end) {
							int tmp = begin;
							begin = end;
							end = tmp;
						}

						string mutable = ((string)((Literal)callee).value).Remove(begin, end - begin + 1);
						mutable = mutable.Insert(begin, (string)value);
						(((Literal)callee).value) = (object)mutable;
					}
				}
			}

			public static object SliceNotationVariable(Expr callee, Token token, Interpreter interpreter, object first, object second, object third) {
				//bounds checking
				if (!(first is double) || ((double)first != double.NegativeInfinity && (double)first < 0) || ((double)first != double.NegativeInfinity && (double)first >= ((string)interpreter.LookupVariable(callee)).Length)) {
					throw new ErrorHandler.RuntimeError(token, "First index must be a number and between 0 and String.Length(string) -1 (inclusive)");
				}

				if (second != null) {
					if (!(second is double) || ((double)second != double.PositiveInfinity && (double)second < 0) || ((double)second != double.PositiveInfinity && (double)second >= ((string)interpreter.LookupVariable(callee)).Length)) {
						throw new ErrorHandler.RuntimeError(token, "Second index must be a number and between 0 and String.Length(string) -1 (inclusive)");
					}
				}

				if (second == null) {
					//access this character only
					return ((string)interpreter.LookupVariable(callee))[(int)(double)first];
				}

				//default values for slice notation (begin and end are inclusive)
				int begin = (double)first == double.NegativeInfinity ? 0 : (int)(double)first;
				int end = (double)second == double.PositiveInfinity ? ((string)interpreter.LookupVariable(callee)).Length - 1 : (int)(double)second;
				int step = third == null ? 1 : (int)(double)third;

				//check for infinite loops
				if (step == 0) {
					throw new ErrorHandler.RuntimeError(token, "Can't have a string step of 0");
				}

				//build the new string
				string newString = "";
				for (int index = step > 0 ? begin : end; index >= begin && index <= end; index += step) {
					newString += ((string)interpreter.LookupVariable(callee))[index];
				}

				return new StringVariableAssignableIndex(callee, newString, interpreter, begin, end, token, third);
			}

			public class StringVariableAssignableIndex : AssignableIndex {
				Expr callee;
				string newString;
				Interpreter interpreter;
				int begin;
				int end;
				Token token;
				object third;

				public StringVariableAssignableIndex(Expr callee, string newString, Interpreter interpreter, int begin, int end, Token token, object third) {
					this.callee = callee;
					this.newString = newString;
					this.interpreter = interpreter;
					this.begin = begin;
					this.end = end;
					this.token = token;
					this.third = third;
				}

				public override object Value {
					get {
						return newString;
					}
					set {
						//disallow a third index
						if (third != null) {
							throw new ErrorHandler.RuntimeError(token, "Unexpected third slice index found");
						}

						//switch
						if (begin > end) {
							int tmp = begin;
							begin = end;
							end = tmp;
						}

						string mutable = ((string)interpreter.LookupVariable(callee)).Remove(begin, end - begin + 1);
						mutable = mutable.Insert(begin, (string)value);
						interpreter.AssignVariable(callee, (object)mutable);
					}
				}
			}

			//IBundle
			public object Property(Interpreter interpreter, Token token, object argument) {
				string propertyName = (string)argument;

				switch(propertyName) {
					case "Length": return new Length(this);
					case "ToLower": return new ToLower(this);
					case "ToUpper": return new ToUpper(this);
					case "Replace": return new Replace(this);
					case "Trim": return new Trim(this);
					case "IndexOf": return new IndexOf(this);
					case "LastIndexOf": return new LastIndexOf(this);

					default:
						throw new ErrorHandler.RuntimeError(token, "Unknown property '" + propertyName + "'");
				}
			}

			public class Length : ICallable {
				String self = null;

				public Length(String self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];

					return str.Length;
				}
			}

			public class ToLower : ICallable {
				String self = null;

				public ToLower(String self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];

					return str.ToLower();
				}
			}

			public class ToUpper : ICallable {
				String self = null;

				public ToUpper(String self) {
					this.self = self;
				}

				public int Arity() {
					return 1;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];

					return str.ToUpper();
				}
			}

			public class Replace : ICallable {
				String self = null;

				public Replace(String self) {
					this.self = self;
				}

				public int Arity() {
					return 3;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];
					string pat = (string)arguments[1];
					string rep = (string)arguments[2];

					return str.Replace(pat, rep);
				}
			}

			public class Trim : ICallable {
				String self = null;

				public Trim(String self) {
					this.self = self;
				}

				public int Arity() {
					return 2;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];
					string chs = (string)arguments[1];

					return str.Trim(chs.ToCharArray());
				}
			}

			public class IndexOf : ICallable {
				String self = null;

				public IndexOf(String self) {
					this.self = self;
				}

				public int Arity() {
					return 2;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];
					string other = (string)arguments[1];

					return str.IndexOf(other);
				}
			}

			public class LastIndexOf : ICallable {
				String self = null;

				public LastIndexOf(String self) {
					this.self = self;
				}

				public int Arity() {
					return 2;
				}

				public object Call(Interpreter interpreter, Token token, List<object> arguments) {
					string str = (string)arguments[0];
					string other = (string)arguments[1];

					return str.LastIndexOf(other);
				}
			}
		}
	}
}