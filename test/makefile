CC=gcc

#for the test
OUT=../testrunner

IDIR =../include
CFLAGS+=$(addprefix -I,$(IDIR)) -Wall -W -pedantic
LIBS=

ifeq ($(OS),Windows_NT)
	CFLAGS+=-DPLATFORM_WINDOWS
endif

ODIR=obj
SRC = $(wildcard *.c)
OBJ = $(addprefix $(ODIR)/,$(SRC:.c=.o))

all: $(OBJ)
	$(CC) -g -o $(OUT) $^ $(CFLAGS) $(LIBS)

$(OBJ): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

$(ODIR)/%.o: %.c
	$(CC) -g -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)
