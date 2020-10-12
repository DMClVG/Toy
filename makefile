export RM=del /S /Q
export OUT = ../out

all:
	$(MAKE) -C source

clean:
	$(MAKE) -C source clean

rebuild: clean all