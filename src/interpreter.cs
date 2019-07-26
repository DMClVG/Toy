using System;

using static Toy.TokenType;

namespace Toy {
	class Interpreter : ExprVisitor<object> {
		//access
		public int Interpret(Expr expr) {
			try {
				object value = Evaluate(expr);
				Console.WriteLine(value); //tmp
			} catch(ErrorHandler.RuntimeError e) {
				throw e;
			} catch(Exception e) {
				throw new ErrorHandler.RuntimeError(new Token(EOF, "", null, -1), e.Message);
			}

			return 0;
		}

		//visitor pattern
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
					return (double)left / (double)right;

				case MODULO:
					CheckNumberOperands(expr.oper, left, right);
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