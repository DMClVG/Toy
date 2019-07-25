using System;

namespace Toy {
	class ErrorHandler {
		//variables
		public static bool HadError { get; private set; } = false;

		//error handling
		public static void Error(int line, string message) {
			Report(line, "", message);
		}

		private static void Report(int line, string where, string message) {
			Console.Error.WriteLine($"[line {line}] Error {where}: {message}");
			HadError = true;
		}

		public static void ResetError() {
			HadError = false;
		}
	}
}