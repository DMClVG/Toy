#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TOY_VERSION_MAJOR 0
#define TOY_VERSION_MINOR 6
#define TOY_VERSION_PATCH 0
#define TOY_VERSION_BUILD __DATE__ " " __TIME__

//for processing the command line arguments
typedef struct {
	bool error;
	bool help;
	bool version;
	char* filename;
	char* source;
	bool verbose;
	int optimize;
} Command;

extern Command command;

void initCommand(int argc, const char* argv[]);

void usageCommand(int argc, const char* argv[]);
void helpCommand(int argc, const char* argv[]);
void copyrightCommand(int argc, const char* argv[]);