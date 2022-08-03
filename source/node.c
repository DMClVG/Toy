#include "node.h"

#include "memory.h"

#include <stdio.h>

void freeNode(Node* node) {
	switch(node->type) {
		case NODE_ATOMIC:
			freeLiteral(node->atomic.literal);
			break;

		case NODE_UNARY:
			freeNode(node->unary.child);
			break;

		case NODE_BINARY:
			freeNode(node->binary.left);
			freeNode(node->binary.right);
			break;
	}

	FREE(Node, node);
}

void emitAtomicLiteral(Node** nodeHandle, Literal literal) {
	//allocate a new node
	*nodeHandle = ALLOCATE(Node, 1);

	(*nodeHandle)->type = NODE_ATOMIC;
	(*nodeHandle)->atomic.literal = literal;
}

void printNode(Node* node) {
	switch(node->type) {
		case NODE_ATOMIC:
			printf("atomic:");
			printLiteral(node->atomic.literal);
			break;
		
		case NODE_UNARY:
			printf("unary:");
			printNode(node->unary.child);
			break;

		case NODE_BINARY:
			printf("binary left:");
			printNode(node->binary.left);
			printf("binary right:");
			printNode(node->binary.right);
			break;
	}
}