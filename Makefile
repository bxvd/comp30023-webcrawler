SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

CC = gcc
CFLAGS = -Wall -I$(IDIR)

OBJ := crawler.o
OBJ := $(ODIR)/$(OBJ)

EXE = crawler

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -r $(ODIR)
