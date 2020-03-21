# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -I$(IDIR)

# Objects and source files
OBJ := crawler.o
SRC := $(patsubst %.o, $(SDIR)/%.c, $(OBJ))
OBJ := $(patsubst %, $(ODIR)/%, $(OBJ))

# Output
EXE = crawler

# Runtime args
# Used for debugging
RUN = http://ibdhost.com/help/html/

.PHONY: all run clean

all: $(EXE)

run: all
	@./$(EXE) $(RUN)

clean:
	@rm -r -f $(ODIR) $(EXE)

$(OBJ): $(SRC)
	@mkdir $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)
