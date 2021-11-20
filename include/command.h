#ifndef TOY_COMMAND_H_
#define TOY_COMMAND_H_

#include "common.h"

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

#endif //TOY_COMMAND_H_