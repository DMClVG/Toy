ifeq ($(OS),Windows_NT)
	export RM=del /S /Q
else ifeq ($(shell uname), Linux)
	export RM=rm -r
endif

export OUT = ../Toy

all:
	$(MAKE) -C source

test: all
	$(MAKE) -C test

clean:
	$(MAKE) -C source clean
	$(MAKE) -C test clean

rebuild: clean all