# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

LIBS = libgumbo.a

# Compiler
CC = clang
CFLAGS = -Wall -Wextra -I$(IDIR)
CMFLAGS = -fsanitize=address -g -O1 -Wall -Wextra -v -I$(IDIR)

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
TEST3 = https://webhook.site/a6e635ec-e82e-4ef8-b94c-20820b1d823e

.PHONY: all run clean

all: clean $(EXE)

test1: all
	@./$(EXE) $(TEST1)

test2: all
	@./$(EXE) $(TEST2)

test3: all
	@./$(EXE) $(TEST3)

clean:
	@rm -r -f $(ODIR) $(EXE)

$(ODIR)/%.o: $(SDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< $(CMFLAGS)

$(EXE): $(OBJ)
	@$(CC) $(LDIR)/$(LIBS) -o $@ $^ $(CMFLAGS)
