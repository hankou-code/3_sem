IDIR=src
CC=gcc
CFLAGS=-I$(IDIR)
BIN=plot


SDIR=src
ODIR=obj

LIBS=-lm -lpthread


_DEPS = calculator.h handler.h explorer.h config.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


_OBJ = calculator.o handler.o explorer.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: $(BIN)


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean


clean:
	rm -f $(ODIR)/*.o $(SDIR)/*~ $(INCDIR)/*~