using CSString = System.String;

namespace Toy {
	namespace Library {
		class String : IPlugin {
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

			//TODO: string.Length()

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
		}
	}
}