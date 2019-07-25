using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Toy {
	class Interpreter {
		//variables
		static bool hadError = false;

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

			if (hadError) {
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
				if (hadError) {
					hadError = false;
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

		//error handling
		public static void Error(int line, string message) {
			Report(line, "", message);
		}

		private static void Report(int line, string where, string message) {
			Console.Error.WriteLine($"[line {line}] Error {where}: {message}");
			hadError = true;
		}
	}
}