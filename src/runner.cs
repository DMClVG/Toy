using System;
using System.IO;
using System.Text;
using System.Collections.Generic;

namespace Toy {
	class Runner {
		public static Environment RunFile(string filename) {
			Environment env = Run(File.ReadAllText(filename, Encoding.UTF8));

			if (ErrorHandler.HadError) {
				return null;
			}

			return env;
		}

		public static Environment Run(string source) {
			try {
				Scanner scanner = new Scanner(source);
				Parser parser = new Parser(scanner.ScanTokens());
				List<Stmt> stmtList = parser.ParseStatements();

				if (ErrorHandler.HadError) {
					return null;
				}

				Interpreter interpreter = new Interpreter();

				Resolver resolver = new Resolver(interpreter);
				resolver.Resolve(stmtList);

				if (ErrorHandler.HadError) {
					return null;
				}

				interpreter.Interpret(stmtList);

				//return the environment context for the import keyword
				return interpreter.environment;

			} catch(ErrorHandler.AssertError) {
				Console.WriteLine("Assert error caught at Run()");
				Console.WriteLine("The program will now exit early");
			} catch (ErrorHandler.ParserError e) {
				Console.WriteLine("Parser error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only, and will be removed from the final release:\n" + e.ToString());
			} catch (ErrorHandler.ResolverError e) {
				Console.WriteLine("Resolver error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only, and will be removed from the final release:\n" + e.ToString());
			} catch (ErrorHandler.RuntimeError e) {
				Console.WriteLine("Runtime error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only, and will be removed from the final release:\n" + e.ToString());
			} catch (Exception e) {
				Console.WriteLine("Terminal error caught at Run()");
				Console.WriteLine("The following output is for internal debugging only, and will be removed from the final release:\n" + e.ToString());
			}

			return null;
		}
	}
}