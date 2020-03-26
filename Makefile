# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

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
RUN = http://ibdhost.com/help/html/

.PHONY: all run clean

all: clean $(EXE)

run: all
	@./$(EXE) $(RUN)

clean:
	@rm -r -f $(ODIR) $(EXE)

$(ODIR)/%.o: $(SDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)
