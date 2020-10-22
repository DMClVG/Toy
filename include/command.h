#ifndef TOY_COMMAND_H
#define TOY_COMMAND_H

#include "common.h"

//DOCS: for command-line arugments

typedef struct {
	bool error;
	bool help;
	bool version;
	char* fname;
	char* source;
	bool verbose;
} Command;

extern Command command;

void initCommand(int argc, const char* argv[]);

void usageCommand(int argc, const char* argv[]);
void helpCommand(int argc, const char* argv[]);
void copyrightCommand(int argc, const char* argv[]);

#endif