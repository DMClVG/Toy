#include "node.h"

#include "memory.h"

#include <stdio.h>

void freeNode(Node* node) {
	//don't free a NULL node
	if (node == NULL) {
		return;
	}

	switch(node->type) {
		case NODE_LITERAL:
			freeLiteral(node->atomic.literal);
			break;

		case NODE_UNARY:
			freeNode(node->unary.child);
			break;

		case NODE_BINARY:
			freeNode(node->binary.left);
			freeNode(node->binary.right);
			break;

		case NODE_GROUPING:
			freeNode(node->grouping.child);
			break;
	}

	FREE(Node, node);
}

void emitNodeLiteral(Node** nodeHandle, Literal literal) {
	//allocate a new node
	*nodeHandle = ALLOCATE(Node, 1);

	(*nodeHandle)->type = NODE_LITERAL;
	(*nodeHandle)->atomic.literal = literal;
}

void emitNodeUnary(Node** nodeHandle, Opcode opcode) {
	//allocate a new node
	*nodeHandle = ALLOCATE(Node, 1);

	(*nodeHandle)->type = NODE_UNARY;
	(*nodeHandle)->unary.opcode = opcode;
	(*nodeHandle)->unary.child = NULL;
}

void emitNodeBinary(Node** nodeHandle, Node* rhs, Opcode opcode) {
	Node* tmp = ALLOCATE(Node, 1);

	tmp->type = NODE_BINARY;
	tmp->binary.opcode = opcode;
	tmp->binary.left = *nodeHandle;
	tmp->binary.right = rhs;

	*nodeHandle = tmp;
}

void emitNodeGrouping(Node** nodeHandle) {
	Node* tmp = ALLOCATE(Node, 1);

	tmp->type = NODE_GROUPING;
	tmp->grouping.child = NULL;

	*nodeHandle = tmp;
}

void printNode(Node* node) {
	switch(node->type) {
		case NODE_LITERAL:
			printf("literal:");
			printLiteral(node->atomic.literal);
			break;

		case NODE_UNARY:
			printf("unary:");
			printNode(node->unary.child);
			break;

		case NODE_BINARY:
			printf("binary-left:");
			printNode(node->binary.left);
			printf("binary-right:");
			printNode(node->binary.right);
			printf(";");
			break;

		case NODE_GROUPING:
			printf("(");
			printNode(node->grouping.child);
			printf(")");
			break;
	}
}