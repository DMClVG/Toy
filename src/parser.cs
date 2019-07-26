using System.Collections.Generic;

using static Toy.TokenType;

namespace Toy {
	class Parser {
		readonly List<Token> tokenList;
		int current = 0;

		public Parser(List<Token> tokens) {
			tokenList = tokens;
		}

		public Expr ParseTokens() {
			try {
				return Expression();
			} catch(ErrorHandler.ParserError) {
				return null;
			}
		}

		//grammar rules
		Expr Expression() {
			Expr expr = Or();

			//handle ternary operator
			if (Match(QUESTION)) {
				Expr left = Expression();
				Consume(COLON, "Expected ':' in ternary operator");
				Expr right = Expression();
				expr = new Ternary(expr, left, right);
			}

			return expr;
		}

		Expr Or() {
			Expr expr = And();

			if (Match(OR_OR)) {
				Token token = Previous();
				Expr right = Or();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr And() {
			Expr expr = Equality();

			if (Match(AND_AND)) {
				Token token = Previous();
				Expr right = And();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr Equality() {
			Expr expr = Comaprison();

			while(Match(EQUAL_EQUAL, BANG_EQUAL)) {
				Token token = Previous();
				Expr right = Comaprison();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr Comaprison() {
			Expr expr = Addition();

			while(Match(LESS, GREATER, LESS_EQUAL, GREATER_EQUAL)) {
				Token token = Previous();
				Expr right = Addition();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr Addition() {
			Expr expr = Multiplication();

			while(Match(PLUS, MINUS)) {
				Token token = Previous();
				Expr right = Multiplication();
				expr = new Binary(expr, token, right);
			}

			return expr;
		}

		Expr Multiplication() {
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

			return Primary();
		}

		Expr Primary() {
			if (Match(TRUE)) return new Literal(true);
			if (Match(FALSE)) return new Literal(false);
			if (Match(NIL)) return new Literal(null);

			if (Match(NUMBER, STRING)) {
				return new Literal(Previous().literal);
			}

			if (Match(LEFT_PAREN)) {
				Expr expr = Expression();
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