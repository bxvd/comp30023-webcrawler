# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

# Sanitise flags, use with gcc
SANITISE = -fsanitize=address -g -O1

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -I$(IDIR)

MKDIR = mkdir -p

# Libraries
LIB := gumbo/src
LIBDIR := $(LDIR)/$(LIB)
LIBO := attribute.o char_ref.o error.o parser.o string_buffer.o string_piece.o tag.o tokenizer.o utf8.o \
				util.o vector.o

# Objects
OBJ := $(LIBO) crawler.o crawl.o http.o url.o client.o page.o
OBJ := $(OBJ:%=$(ODIR)/%)
LIBO := $(LIBO:%=$(ODIR)/%)

# Output
EXE = crawler

# Look in lib for extra header files
vpath %.h $(LIBDIR)

.PHONY: all run clean tidy libs gumbo

all: clean libs $(EXE)

clean:
	@rm -r -f $(ODIR) $(EXE)

tidy:
	@rm -r -f $(ODIR)

libs: gumbo

# Build Gumbo library from src
gumbo: $(LIBO)

# Build src files
$(ODIR)/%.o: $(SDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

# Build lib src files
$(ODIR)/%.o: $(LIBDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< -I$(IDIR)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)
