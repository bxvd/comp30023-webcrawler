# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

# Compiler
CC = gcc
CFLAGS = -Wall -I$(IDIR)

# Objects
OBJ := crawler.o
OBJ := $(patsubst %, $(ODIR)/%, $(OBJ))

# Output
EXE = crawler

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	@rm -r -f $(ODIR) $(EXE)
