namespace Toy {
	abstract class Expr {
		public abstract R Accept<R>(ExprVisitor<R> visitor);
	}

	interface ExprVisitor<R> {
		R Visit(Literal Expr);
		R Visit(Unary Expr);
		R Visit(Binary Expr);
		R Visit(Grouping Expr);
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

	class Grouping : Expr {
		public Grouping(Expr expression) {
			this.expression = expression;
		}

		public override R Accept<R>(ExprVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr expression;
	}

}
