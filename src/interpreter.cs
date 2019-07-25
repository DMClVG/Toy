using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Toy {
	class Interpreter {
		static void Main() {
			Expr expr = new Binary(
				new Unary(
					new Token(TokenType.MINUS, "-", null, 1),
					new Literal(123)
				),
				new Token(TokenType.STAR, "*", null, 1),
				new Grouping(
					new Literal(45.67)
				)
			);

			AstPrinter printer = new AstPrinter();
			Console.WriteLine(printer.Print(expr));
		}

		//static methods
		static void NotMain(string[] args) {
			//get the app's name
			string appName = System.Reflection.Assembly.GetExecutingAssembly().GetName().Name;

			if (args.Length > 1) {
				Console.Write($"Usage: {appName} [script]");
			} else if (args.Length == 1) {
				RunFile(args[0]);
			} else {
				RunPrompt();
			}
		}

		static void RunFile(string filename) {
			Run(File.ReadAllText(filename, Encoding.UTF8));

			if (ErrorHandler.HadError) {
				Environment.Exit(-1);
			}
		}

		static void RunPrompt() {
			string input;

			while(true) {
				Console.Write(">");
				input = Console.ReadLine();
				Run(input);

				//ignore errors in prompt mode
				if (ErrorHandler.HadError) {
					ErrorHandler.ResetError();
				}
			}
		}

		static void Run(string source) {
			Scanner scanner = new Scanner(source);
			List<Token> tokenList = scanner.ScanTokens();

			foreach (Token token in tokenList) {
				Console.WriteLine(token);
			}
		}
	}
}