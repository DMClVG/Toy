using System.Collections.Generic;

using static Toy.TokenType;

namespace Toy {
	class Parser {
		readonly List<Token> tokenList;
		int current = 0;

		public Parser(List<Token> tokens) {
			tokenList = tokens;
		}

		public List<Stmt> ParseStatements() {
			List<Stmt> statements = new List<Stmt>();
			while(!CheckAtEnd()) {
				statements.Add(DeclarationRule());
			}
			return statements;
		}

		//grammar rules
		Stmt DeclarationRule() {
			try {
				if (Match(VAR)) return VarDeclarationRule();
				if (Match(CONST)) return ConstDeclarationRule();

				return StatementRule();
			} catch(ErrorHandler.ParserError) {
				Synchronize();
				return null;
			}
		}

		Stmt VarDeclarationRule() {
			Token name = Consume(IDENTIFIER, "Expected variable name");

			Expr initializer = null;
			if (Match(EQUAL)) {
				initializer = ExpressionRule();
			}

			Consume(SEMICOLON, "Expected ';' after variable declaration");
			return new Var(name, initializer);
		}

		Stmt ConstDeclarationRule() {
			Token name = Consume(IDENTIFIER, "Expected constant name");
			Consume(EQUAL, "Expected assignment in constant declaration");
			Expr initializer = ExpressionRule();
			Consume(SEMICOLON, "Expected ';' after constant declaration");
			return new Const(name, initializer);
		}

		Stmt StatementRule() {
			if (Match(PRINT)) return PrintStmt();
			if (Match(LEFT_BRACE)) return new Block(BlockStmt());

			return ExpressionStmt();
		}

		Stmt PrintStmt() {
			Expr expr = ExpressionRule();
			Consume(SEMICOLON, "Expected ';' after value");
			return new Print(expr);
		}

		Stmt ExpressionStmt() {
			Expr expr = ExpressionRule();
			Consume(SEMICOLON, "Expected ';' after expression");
			return new Expression(expr);
		}

		List<Stmt> BlockStmt() {
			List<Stmt> statements = new List<Stmt>();

			while(Peek().type != RIGHT_BRACE && !CheckAtEnd()) {
				statements.Add(DeclarationRule());
			}

			Consume(RIGHT_BRACE, "Expected '}' after block");
			return statements;
		}

		Expr ExpressionRule() {
			return AssignmentRule();
		}

		Expr AssignmentRule() {
			Expr expr = TernaryRule();

			if (Match(EQUAL, PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MODULO_EQUAL)) {
				Token token = Previous();
				Expr value = AssignmentRule();

				if (expr is Variable) {
					Token name = ((Variable)expr).name;
					return new Assign(name, token, value);
				}

				throw new ErrorHandler.ParserError(token, "Invalid assignment target");
			}

			return expr;
		}

		Expr TernaryRule() {
			Expr expr = OrRule();

			//handle ternary operator
			if (Match(QUESTION)) {
				Expr left = ExpressionRule();
				Consume(COLON, "Expected ':' in ternary operator");
				Expr right = ExpressionRule();
				expr = new Ternary(expr, left, right);
			}

			return expr;
		}

		Expr OrRule() {
			Expr expr = AndRule();

			if (Match(OR_OR)) {
				Token token = Previous();
				Expr right = OrRule();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr AndRule() {
			Expr expr = EqualityRule();

			if (Match(AND_AND)) {
				Token token = Previous();
				Expr right = AndRule();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr EqualityRule() {
			Expr expr = ComaprisonRule();

			while(Match(EQUAL_EQUAL, BANG_EQUAL)) {
				Token token = Previous();
				Expr right = ComaprisonRule();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr ComaprisonRule() {
			Expr expr = AdditionRule();

			while(Match(LESS, GREATER, LESS_EQUAL, GREATER_EQUAL)) {
				Token token = Previous();
				Expr right = AdditionRule();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr AdditionRule() {
			Expr expr = MultiplicationRule();

			while(Match(PLUS, MINUS)) {
				Token token = Previous();
				Expr right = MultiplicationRule();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr MultiplicationRule() {
			Expr expr = UnaryRule();

			while(Match(STAR, SLASH, MODULO)) {
				Token token = Previous();
				Expr right = UnaryRule();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr UnaryRule() {
			if (Match(BANG, MINUS)) {
				Token token = Previous();
				Expr right = UnaryRule();
				return new Unary(token, right);
			}

			return PrefixRule();
		}

		Expr PrefixRule() {
			if (Match(PLUS_PLUS, MINUS_MINUS)) {
				Token token = Previous();
				Consume(IDENTIFIER, "Expected identifier after prefix increment/decrement operator");
				Variable variable = new Variable(Previous());
				return new Increment(token, variable, true);
			}

			return PostfixRule();
		}

		Expr PostfixRule() {
			if (Peek().type == IDENTIFIER && (PeekNext().type == PLUS_PLUS || PeekNext().type == MINUS_MINUS)) {
				Variable variable = new Variable(Advance());
				Token token = Advance();
				return new Increment(token, variable, false);
			}

			return PrimaryRule();
		}

		Expr PrimaryRule() {
			if (Match(TRUE)) return new Literal(true);
			if (Match(FALSE)) return new Literal(false);
			if (Match(NIL)) return new Literal(null);

			if (Match(NUMBER, STRING)) {
				return new Literal(Previous().literal);
			}

			if (Match(IDENTIFIER)) {
				return new Variable(Previous()); //Variable accesses constants as well
			}

			if (Match(LEFT_PAREN)) {
				Expr expr = ExpressionRule();
				Consume(RIGHT_PAREN, "Expected ')' after expression");
				return new Grouping(expr);
			}

			throw new ErrorHandler.ParserError(Peek(), "Expected expression");
		}

		//helpers
		bool Match(params TokenType[] types) {
			foreach (TokenType type in types) {
				if (CheckTokenType(type)) {
					Advance();
					return true;
				}
			}

			return false;
		}

		bool CheckTokenType(TokenType type) {
			if (CheckAtEnd()) {
				return false;
			}
			return Peek().type == type;
		}

		bool CheckAtEnd() {
			return Peek().type == EOF;
		}

		Token Peek() {
			return tokenList[current];
		}

		Token PeekNext() {
			return tokenList[current + 1];
		}

		Token Previous() {
			return tokenList[current - 1];
		}

		Token Advance() {
			if (!CheckAtEnd()) {
				current++;
			}
			return Previous();
		}

		Token Consume(TokenType type, string message) {
			if (CheckTokenType(type)) {
				return Advance();
			}

			throw new ErrorHandler.ParserError(Peek(), message);
		}

		//error handling
		void Synchronize() {
			Advance();

			while (!CheckAtEnd()) {
				if (Previous().type == SEMICOLON) return;

				switch(Peek().type) {
					//TODO: handle arrow functions (they're expressions, I think)
					case PRINT:
					case IMPORT:
					case VAR:
					case CONST:
					case FUNCTION:
					case IF:
					case DO:
					case WHILE:
					case FOR:
					case FOREACH:
						return;
				}

				Advance();
			}
		}
	}
}