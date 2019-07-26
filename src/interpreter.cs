using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Toy {
	class Interpreter {
		//static methods
		static void Main(string[] args) {
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
			Parser parser = new Parser(scanner.ScanTokens());
			Expr expression = parser.ParseTokens();

			if (ErrorHandler.HadError) {
				return;
			}

			AstPrinter printer = new AstPrinter();
			Console.WriteLine(printer.Print(expression));
		}
	}
}