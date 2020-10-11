CC=gcc

IDIR =../include
CFLAGS=$(addprefix -I,$(IDIR)) # -Wall -W -pedantic

ODIR=obj

LIBS=

SRC = $(wildcard *.c)
OBJ = $(SRC: .c=.o)

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	$(CC) -o $(OUT) $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o