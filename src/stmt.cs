using System.Collections.Generic;

namespace Toy {
	abstract class Stmt {
		public abstract R Accept<R>(StmtVisitor<R> visitor);
	}

	interface StmtVisitor<R> {
		R Visit(Print Stmt);
		R Visit(If Stmt);
		R Visit(While Stmt);
		R Visit(For Stmt);
		R Visit(Break Stmt);
		R Visit(Continue Stmt);
		R Visit(Block Stmt);
		R Visit(Var Stmt);
		R Visit(Const Stmt);
		R Visit(Expression Stmt);
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

	class If : Stmt {
		public If(Expr cond, Stmt thenBranch, Stmt elseBranch) {
			this.cond = cond;
			this.thenBranch = thenBranch;
			this.elseBranch = elseBranch;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr cond;
		public Stmt thenBranch;
		public Stmt elseBranch;
	}

	class While : Stmt {
		public While(Expr cond, Stmt body) {
			this.cond = cond;
			this.body = body;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr cond;
		public Stmt body;
	}

	class For : Stmt {
		public For(Stmt initializer, Expr cond, Expr increment, Stmt body) {
			this.initializer = initializer;
			this.cond = cond;
			this.increment = increment;
			this.body = body;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Stmt initializer;
		public Expr cond;
		public Expr increment;
		public Stmt body;
	}

	class Break : Stmt {
		public Break(Token signal) {
			this.signal = signal;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token signal;
	}

	class Continue : Stmt {
		public Continue(Token signal) {
			this.signal = signal;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Token signal;
	}

	class Block : Stmt {
		public Block(List<Stmt> statements, bool breakable) {
			this.statements = statements;
			this.breakable = breakable;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public List<Stmt> statements;
		public bool breakable;
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

	class Expression : Stmt {
		public Expression(Expr expression) {
			this.expression = expression;
		}

		public override R Accept<R>(StmtVisitor<R> visitor) {
			return visitor.Visit(this);
		}

		public Expr expression;
	}

}
