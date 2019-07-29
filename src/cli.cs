using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Toy {
	class CLI {
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
				System.Environment.Exit(-1);
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
			try {
				Scanner scanner = new Scanner(source);
				Parser parser = new Parser(scanner.ScanTokens());
				List<Stmt> stmtList = parser.ParseStatements();

				if (ErrorHandler.HadError) {
					return;
				}

				Interpreter interpreter = new Interpreter();

				Resolver resolver = new Resolver(interpreter);
				resolver.Resolve(stmtList);

				if (ErrorHandler.HadError) {
					return;
				}

				interpreter.Interpret(stmtList);

			} catch (ErrorHandler.ParserError e) {
				Console.WriteLine("Parser error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only:\n" + e.ToString());
			} catch (ErrorHandler.ResolverError e) {
				Console.WriteLine("Resolver error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only:\n" + e.ToString());
			} catch (ErrorHandler.RuntimeError e) {
				Console.WriteLine("Runtime error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only:\n" + e.ToString());
			} catch (Exception e) {
				Console.WriteLine("Terminal error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only:\n" + e.ToString());
			}
		}
	}
}