#config
CSC=csc

#output
OUTDIR=../out

#targets
all: generate_ast

generate_ast:
	$(CSC) -out:$(OUTDIR)/generate_ast.exe generate_ast.cs
	$(OUTDIR)/generate_ast.exe ../src

clean:
ifeq ($(OS),Windows_NT)
	$(RM) *.o *.a *.exe
else ifeq ($(shell uname), Linux)
	find . -type f -name *.o -exec rm -f -r -v {} \;
	find . -type f -name *.a -exec rm -f -r -v {} \;
endif

rebuild: clean all
