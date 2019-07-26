using System.Collections.Generic;

namespace Toy {
	abstract class Stmt {
		public abstract R Accept<R>(StmtVisitor<R> visitor);
	}

	interface StmtVisitor<R> {
		R Visit(Expression Stmt);
		R Visit(Block Stmt);
		R Visit(Print Stmt);
		R Visit(Var Stmt);
		R Visit(Const Stmt);
	}

	class Expression : Stmt {
		public Expression(Expr expression) {
			this.expression = expression;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr expression;
	}

	class Block : Stmt {
		public Block(List<Stmt> statements) {
			this.statements = statements;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public List<Stmt> statements;
	}

	class Print : Stmt {
		public Print(Expr expression) {
			this.expression = expression;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr expression;
	}

	class Var : Stmt {
		public Var(Token name, Expr initializer) {
			this.name = name;
			this.initializer = initializer;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token name;
		public Expr initializer;
	}

	class Const : Stmt {
		public Const(Token name, Expr initializer) {
			this.name = name;
			this.initializer = initializer;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token name;
		public Expr initializer;
	}

}
