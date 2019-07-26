using System;
using System.IO;

namespace Tool {
	class GenerateAst {
		static int Main(string[] args) {
			if (args.Length != 1) {
				Console.Error.WriteLine("Usage: generate_ast <outdir>");
				return -1;
			}

			DefineAst(args[0], "Expr", new string[] {
				"Variable: Token name",
				"Assign: Token name, Token oper, Expr value",
				"Increment: Token oper, Variable variable, bool prefix",
				"Literal: object value",
				"Unary: Token oper, Expr right",
				"Binary: Expr left, Token oper, Expr right",
				"Grouping: Expr expression",
				"Ternary: Expr cond, Expr left, Expr right"
			});

			DefineAst(args[0], "Stmt", new string[] {
				"Expression: Expr expression",
				"Print: Expr expression",
				"Var: Token name, Expr initializer",
				"Const: Token name, Expr initializer"
			});

			return 0;
		}

		static void DefineAst(string outDir, string baseName, string[] types) {
			StreamWriter outStream = new StreamWriter($"{outDir}/{baseName.ToLower()}.cs");

			//the namespace
			outStream.WriteLine("namespace Toy {");

			//the base class
			outStream.WriteLine("\tabstract class " + baseName + " {");
			outStream.WriteLine("\t\tpublic abstract R Accept<R>(" + baseName + "Visitor<R> visitor);");
			outStream.WriteLine("\t}");
			outStream.WriteLine("");

			//the visitor interface
			DefineVisitor(outStream, baseName, types);

			outStream.WriteLine("");

			//each type
			foreach(string type in types) {
				string[] arr = type.Split(new char[] {':'});
				string className = arr[0].Trim();
				string members = arr[1].Trim();
				DefineType(outStream, baseName, className, members);
			}

			outStream.WriteLine("}");
			outStream.Close();
		}

		static void DefineType(StreamWriter outStream, string baseName, string className, string types) {
			outStream.WriteLine("\tclass " + className + " : " + baseName + " {");

			//get each field
			string[] fields = types.Split(new char[] {','});

			//constructor
			outStream.WriteLine("\t\tpublic " + className + "(" + types + ") {");

			//assign each field
			foreach (string field in fields) {
				string name = field.Trim().Split(new char[] {' '})[1];
				outStream.WriteLine("\t\t\tthis." + name + " = " + name + ";");
			}

			outStream.WriteLine("\t\t}");
			outStream.WriteLine("");

			//visitor pattern
			outStream.WriteLine("\t\tpublic override R Accept<R>(" + baseName + "Visitor<R> visitor) {");
			outStream.WriteLine("\t\t\treturn visitor.Visit(this);");
			outStream.WriteLine("\t\t}");

			outStream.WriteLine("");

			//public members
			foreach (string field in fields) {
				outStream.WriteLine("\t\tpublic " + field.Trim() + ";");
			}

			outStream.WriteLine("\t}");
			outStream.WriteLine("");
		}

		static void DefineVisitor(StreamWriter outStream, string baseName, string[] types) {
			//the visitor interface
			outStream.WriteLine("\tinterface " + baseName + "Visitor<R> {");

			foreach(string type in types) {
				string typeName = type.Split(new char[] {':'})[0].Trim();
				outStream.WriteLine($"\t\tR Visit({typeName} {baseName});");
			}

			outStream.WriteLine("\t}");
		}
	}
}