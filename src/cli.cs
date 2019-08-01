using System;

namespace Toy {
	class CLI {
		//static methods
		static void Main(string[] args) {
			//get the app's name
			string appName = System.Reflection.Assembly.GetExecutingAssembly().GetName().Name;

			if (args.Length > 1) {
				Console.Write($"Usage: {appName} [script]");
			} else if (args.Length == 1) {
				if (Runner.RunFile(args[0]) == null) {
					System.Environment.Exit(-1);
				}
			} else {
				RunPrompt();
			}
		}

		static void RunPrompt() {
			string input;

			while(true) {
				Console.Write(">");
				input = Console.ReadLine();
				Runner.Run(input);

				//ignore errors in prompt mode
				if (ErrorHandler.HadError) {
					ErrorHandler.ResetError();
				}
			}
		}
	}
}