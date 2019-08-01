using System.Collections.Generic;

namespace Toy {
	abstract class Expr {
		public abstract R Accept<R>(ExprVisitor<R> visitor);
	}

	interface ExprVisitor<R> {
		R Visit(Variable Expr);
		R Visit(Assign Expr);
		R Visit(Increment Expr);
		R Visit(Literal Expr);
		R Visit(Logical Expr);
		R Visit(Unary Expr);
		R Visit(Binary Expr);
		R Visit(Call Expr);
		R Visit(Index Expr);
		R Visit(Function Expr);
		R Visit(Property Expr);
		R Visit(Grouping Expr);
		R Visit(Ternary Expr);
	}

	class Variable : Expr {
		public Variable(Token name) {
			this.name = name;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token name;
	}

	class Assign : Expr {
		public Assign(Expr left, Token oper, Expr right) {
			this.left = left;
			this.oper = oper;
			this.right = right;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr left;
		public Token oper;
		public Expr right;
	}

	class Increment : Expr {
		public Increment(Token oper, Variable variable, bool prefix) {
			this.oper = oper;
			this.variable = variable;
			this.prefix = prefix;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token oper;
		public Variable variable;
		public bool prefix;
	}

	class Literal : Expr {
		public Literal(object value) {
			this.value = value;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public object value;
	}

	class Logical : Expr {
		public Logical(Expr left, Token oper, Expr right) {
			this.left = left;
			this.oper = oper;
			this.right = right;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr left;
		public Token oper;
		public Expr right;
	}

	class Unary : Expr {
		public Unary(Token oper, Expr right) {
			this.oper = oper;
			this.right = right;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token oper;
		public Expr right;
	}

	class Binary : Expr {
		public Binary(Expr left, Token oper, Expr right) {
			this.left = left;
			this.oper = oper;
			this.right = right;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr left;
		public Token oper;
		public Expr right;
	}

	class Call : Expr {
		public Call(Expr callee, Token paren, List<Expr> arguments) {
			this.callee = callee;
			this.paren = paren;
			this.arguments = arguments;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr callee;
		public Token paren;
		public List<Expr> arguments;
	}

	class Index : Expr {
		public Index(Expr callee, Expr first, Expr second, Expr third, Token bracket) {
			this.callee = callee;
			this.first = first;
			this.second = second;
			this.third = third;
			this.bracket = bracket;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr callee;
		public Expr first;
		public Expr second;
		public Expr third;
		public Token bracket;
	}

	class Function : Expr {
		public Function(List<Expr> parameters, List<Stmt> body) {
			this.parameters = parameters;
			this.body = body;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public List<Expr> parameters;
		public List<Stmt> body;
	}

	class Property : Expr {
		public Property(Expr expression, Token name) {
			this.expression = expression;
			this.name = name;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr expression;
		public Token name;
	}

	class Grouping : Expr {
		public Grouping(Expr expression) {
			this.expression = expression;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr expression;
	}

	class Ternary : Expr {
		public Ternary(Expr cond, Expr left, Expr right) {
			this.cond = cond;
			this.left = left;
			this.right = right;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr cond;
		public Expr left;
		public Expr right;
	}

}
