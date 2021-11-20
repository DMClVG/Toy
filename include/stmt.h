#ifndef TOY_STMT_H_
#define TOY_STMT_H_

#include "expr.h"

//stmts
typedef enum StmtType {
	STMT_PRINT,
	STMT_DECL_FUNC,
	STMT_DECL_CONST,
	STMT_DECL_VAR,
	STMT_RETURN,
	STMT_IMPORT,
	STMT_EXPORT,
	STMT_WHILE,
	STMT_DO,
	STMT_FOR,
	STMT_BREAK,
	STMT_CONTINUE,
	STMT_IF,
	STMT_ASSERT,
	STMT_PASS,
	STMT_BLOCK,
	STMT_EXPR,
} StmtType;

//forward declare
union Stmt;

//repetitive structures
typedef struct PrintStmt {
	StmtType type; //STMT_PRINT
	//
} PrintStmt;

typedef struct DeclFuncStmt {
	StmtType type; //STMT_DECL_FUNC
	//
} DeclFuncStmt;

typedef struct DeclConstStmt {
	StmtType type; //STMT_DECL_CONST
	//
} DeclConstStmt;

typedef struct DeclVarStmt {
	StmtType type; //STMT_DECL_VAR
	//
} DeclVarStmt;

typedef struct ReturnStmt {
	StmtType type; //STMT_RETURN
	//
} ReturnStmt;

typedef struct ImportStmt {
	StmtType type; //STMT_IMPORT
	//
} ImportStmt;

typedef struct ExportStmt {
	StmtType type; //STMT_EXPORT
	//
} ExportStmt;

typedef struct WhileStmt {
	StmtType type; //STMT_WHILE
	//
} WhileStmt;

typedef struct DoStmt {
	StmtType type; //STMT_DO
	//
} DoStmt;

typedef struct ForStmt {
	StmtType type; //STMT_FOR
	//
} ForStmt;

typedef struct BreakStmt {
	StmtType type; //STMT_BREAK
	//
} BreakStmt;

typedef struct ContinueStmt {
	StmtType type; //STMT_CONTINUE
	//
} ContinueStmt;

typedef struct IfStmt {
	StmtType type; //STMT_IF
	//
} IfStmt;

typedef struct AssertStmt {
	StmtType type; //STMT_ASSERT
	//
} AssertStmt;

typedef struct PassStmt {
	StmtType type; //STMT_PASS
	//
} PassStmt;

typedef struct BlockStmt {
	StmtType type; //STMT_BLOCK
	//
} BlockStmt;

typedef struct ExprStmt {
	StmtType type; //STMT_EXPR
	//
} ExprStmt;

//tagged union
typedef union Stmt {
	StmtType type;

	//repetitive structures
	PrintStmt     printStmt;
	DeclFuncStmt  declFuncStmt;
	DeclConstStmt declConstStmt;
	DeclVarStmt   declVarStmt;
	ReturnStmt    returnStmt;
	ImportStmt    importStmt;
	ExportStmt    exportStmt;
	WhileStmt     whileStmt;
	DoStmt        doStmt;
	ForStmt       forStmt;
	BreakStmt     breakStmt;
	ContinueStmt  continueStmt;
	IfStmt        ifStmt;
	AssertStmt    assertStmt;
	PassStmt      passStmt;
	BlockStmt     blockStmt;
	ExprStmt      exprStmt;
} Stmt;

#endif //TOY_STMT_H_
