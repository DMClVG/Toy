#ifndef TOY_EXPR_H_
#define TOY_EXPR_H_

//exprs
typedef enum ExprType {
	EXPR_ASSIGNMENT,
	EXPR_GROUPING,
	EXPR_TERNARY,
	EXPR_BINARY,
	EXPR_UNARY,
	EXPR_POSTFIX,
	EXPR_PREFIX,
	EXPR_CALL,
	EXPR_INDEX,
	EXPR_PRIMARY,
} ExprType;

//forward declare
union Expr;

//repetitive structures
typedef struct AssignmentExpr {
	ExprType type; //EXPR_ASSIGNMENT
	//
} AssignmentExpr;

typedef struct GroupingExpr {
	ExprType type; //EXPR_GROUPING
	//
} GroupingExpr;

typedef struct TernaryExpr {
	ExprType type; //EXPR_TERNARY
	//
} TernaryExpr;

typedef struct BinaryExpr {
	ExprType type; //EXPR_BINARY
	//
} BinaryExpr;

typedef struct UnaryExpr {
	ExprType type; //EXPR_UNARY
	//
} UnaryExpr;

typedef struct PostfixExpr {
	ExprType type; //EXPR_POSTFIX
	//
} PostfixExpr;

typedef struct PrefixExpr {
	ExprType type; //EXPR_PREFIX
	//
} PrefixExpr;

typedef struct CallExpr {
	ExprType type; //EXPR_CALL
	//
} CallExpr;

typedef struct IndexExpr {
	ExprType type; //EXPR_INDEX
	//
} IndexExpr;

typedef struct PrimaryExpr {
	ExprType type; //EXPR_PRIMARY
	//
} PrimaryExpr;

//tagged union
typedef union Expr {
	ExprType type;

	//repetitive structures
	AssignmentExpr assignmentExpr;
	GroupingExpr   groupingExpr;
	TernaryExpr    ternaryExpr;
	BinaryExpr     binaryExpr;
	UnaryExpr      unaryExpr;
	PostfixExpr    postfixExpr;
	PrefixExpr     prefixExpr;
	CallExpr       callExpr;
	IndexExpr      indexExpr;
	PrimaryExpr    primaryExpr;
} Expr;

#endif //TOY_EXPR_H_
