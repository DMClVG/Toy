using System;
using System.Collections.Generic;

using static Toy.TokenType;

namespace Toy {
	class Interpreter : ExprVisitor<object>, StmtVisitor<object> {
		//members
		Environment environment = new Environment();

		//access
		public int Interpret(List<Stmt> stmtList) {
			try {
				foreach (Stmt stmt in stmtList) {
					Execute(stmt);
				}
			} catch(ErrorHandler.RuntimeError e) {
				throw e;
			} catch(Exception e) {
				throw new ErrorHandler.RuntimeError(new Token(EOF, "", null, -1), e.Message);
			}

			return 0;
		}

		//visitor pattern
		public object Visit(Print stmt) {
			object value = Evaluate(stmt.expression);

			//unescape a string
			if (value is string) {
				Console.WriteLine(System.Text.RegularExpressions.Regex.Unescape((string)value));
			} else if (value is null) {
				Console.WriteLine("null");
			} else {
				Console.WriteLine(value);
			}

			return null;
		}

		public object Visit(Var stmt) {
			object value = null;
			if (stmt.initializer != null) {
				value = Evaluate(stmt.initializer);
			}
			environment.Define(stmt.name, value, false);
			return null;
		}

		public object Visit(Const stmt) {
			environment.Define(stmt.name, Evaluate(stmt.initializer), true);
			return null;
		}

		public object Visit(Expression stmt) {
			Evaluate(stmt.expression);
			return null;
		}

		public object Visit(Variable expr) {
			return environment.Get(expr.name);
		}

		public object Visit(Assign expr) {
			object value = Evaluate(expr.value);
			object originalValue = environment.Get(expr.name);

			switch(expr.oper.type) {
				case EQUAL:
					return environment.Set(expr.name, value);

				case PLUS_EQUAL:
					if (!(originalValue is double)) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't increment-assign a non-number variable");
					}

					return environment.Set(expr.name, (double)originalValue + (double)value);

				case MINUS_EQUAL:
					if (!(originalValue is double)) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't decrement-assign a non-number variable");
					}

					return environment.Set(expr.name, (double)originalValue - (double)value);

				case STAR_EQUAL:
					if (!(originalValue is double)) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't multiply-assign a non-number variable");
					}

					return environment.Set(expr.name, (double)originalValue * (double)value);

				case SLASH_EQUAL:
					if (!(originalValue is double)) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't divide-assign a non-number variable");
					}

					if ((double)originalValue == 0) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't divide by 0");
					}

					return environment.Set(expr.name, (double)originalValue / (double)value);

				case MODULO_EQUAL:
					if (!(originalValue is double)) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't modulo-assign a non-number variable");
					}

					if ((double)originalValue == 0) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't modulo by 0");
					}

					return environment.Set(expr.name, (double)originalValue % (double)value);

				default:
					throw new ErrorHandler.RuntimeError(expr.oper, "Unknown operator");
			}
		}

		public object Visit(Increment expr) {
			object value = environment.Get(expr.variable.name);
			object originalValue = value;

			if (expr.oper.type == PLUS_PLUS) {
				value = (double)value + 1;
			} else if (expr.oper.type == MINUS_MINUS) {
				value = (double)value - 1;
			} else {
				throw new ErrorHandler.RuntimeError(expr.oper, "Bad increment implementation");
			}

			environment.Set(expr.variable.name, value);

			if (expr.prefix) {
				return value;
			} else {
				return originalValue;
			}
		}

		public object Visit(Literal expr) {
			return expr.value;
		}

		public object Visit(Unary expr) {
			object right = Evaluate(expr.right);

			switch(expr.oper.type) {
				case MINUS:
					return -(double)right;

				case BANG:
					return !CheckIsTruthy(right);
			}

			return null;
		}

		public object Visit(Binary expr) {
			object left = Evaluate(expr.left);
			object right = Evaluate(expr.right);

			switch(expr.oper.type) {
				case PLUS:
					if (left is double && right is double) {
						return (double)left + (double)right;
					} else if (left is string && right is string) {
						return (string)left + (string)right;
					} else {
						throw new ErrorHandler.RuntimeError(expr.oper, "Unexpected operand type (expected both numbers or both strings)");
					}

				case MINUS:
					CheckNumberOperands(expr.oper, left, right);
					return (double)left - (double)right;

				case STAR:
					CheckNumberOperands(expr.oper, left, right);
					return (double)left * (double)right;

				case SLASH:
					CheckNumberOperands(expr.oper, left, right);
					if ((double)right == 0) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't divide by 0");
					}
					return (double)left / (double)right;

				case MODULO:
					CheckNumberOperands(expr.oper, left, right);
					if ((double)right == 0) {
						throw new ErrorHandler.RuntimeError(expr.oper, "Can't modulo by 0");
					}
					return (double)left % (double)right;

				case GREATER:
					CheckNumberOperands(expr.oper, left, right);
					return (double)left > (double)right;

				case GREATER_EQUAL:
					CheckNumberOperands(expr.oper, left, right);
					return (double)left >= (double)right;

				case LESS:
					CheckNumberOperands(expr.oper, left, right);
					return (double)left < (double)right;

				case LESS_EQUAL:
					CheckNumberOperands(expr.oper, left, right);
					return (double)left <= (double)right;

				case BANG_EQUAL:
					return !CheckIsEqual(left, right);

				case EQUAL_EQUAL:
					return CheckIsEqual(left, right);

				case AND_AND:
					return CheckIsTruthy(left) && CheckIsTruthy(right);

				case OR_OR:
					return CheckIsTruthy(left) || CheckIsTruthy(right);
			}

			return null;
		}

		public object Visit(Grouping expr) {
			return Evaluate(expr.expression);
		}

		public object Visit(Ternary expr) {
			object cond = Evaluate(expr.cond);
			if (CheckIsTruthy(cond)) {
				return Evaluate(expr.left);
			} else {
				return Evaluate(expr.right);
			}
		}

		//helpers
		void Execute(Stmt stmt) {
			stmt.Accept(this);
		}

		object Evaluate(Expr expr) {
			return expr.Accept(this);
		}

		bool CheckIsTruthy(object obj) {
			if (obj == null) return false;
			if (obj is bool) return (bool)obj;
			if (obj is double) return (double)obj != 0;
			return true;
		}

		bool CheckIsEqual(object left, object right) {
			if (left == null && right == null) {
				return true;
			}

			if (left == null) {
				return false;
			}

			//all numbers but 0 are truthy
			if ((left is double && right is bool) || (left is bool && right is double)) {
				return CheckIsTruthy(left) == CheckIsTruthy(right);
			}

			return left.Equals(right);
		}

		void CheckNumberOperands(Token oper, params object[] operands) {
			foreach(object obj in operands) {
				if (!(obj is double)) {
					throw new ErrorHandler.RuntimeError(oper, "Unexpected operand type (expected a number)");
				}
			}
		}
	}
}