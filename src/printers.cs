namespace Toy {
	class AstPrinter : ExprVisitor<string> {
		public string Print(Expr expr) {
			return expr.Accept(this);
		}

		//visitor pattern
		public string Visit(Literal expr) {
			if (expr.value == null) {
				return "null";
			}

			return expr.value.ToString();
		}

		public string Visit(Unary expr) {
			return Parenthesize(expr.oper.lexeme, expr.right);
		}

		public string Visit(Binary expr) {
			return Parenthesize(expr.oper.lexeme, expr.left, expr.right);
		}

		public string Visit(Grouping expr) {
			return Parenthesize("group", expr.expression);
		}

		//helpers
		string Parenthesize(string name, params Expr[] exprs) {
			string str = "(" + name;
			foreach(Expr expr in exprs) {
				str += " ";
				str += expr.Accept(this);
			}
			str += ")";
			return str;
		}
	}
}