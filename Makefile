# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

LIBS = libgumbo.a

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -I$(IDIR)

MKDIR = mkdir -p

# Objects
OBJ := crawler.o crawl.o http.o url.o client.o
OBJ := $(OBJ:%=$(ODIR)/%)

# Output
EXE = crawler

# Runtime args
# Used for debugging
TEST1 = http://ibdhost.com/help/html/
TEST2 = google.com

.PHONY: all run clean

all: clean $(EXE)

test1: all
	@./$(EXE) $(TEST1)

test2: all
	@./$(EXE) $(TEST2)

clean:
	@rm -r -f $(ODIR) $(EXE)

$(ODIR)/%.o: $(SDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	@$(CC) $(LDIR)/$(LIBS) -o $@ $^ $(CFLAGS)
