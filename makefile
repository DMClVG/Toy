export RM=del /S /Q
export OUT = ../Toy

all:
	$(MAKE) -C source

test: all
	$(MAKE) -C test

clean:
	$(MAKE) -C source clean
	$(MAKE) -C test clean
	$(RM) *.o *.a *.exe

rebuild: clean all