# Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Directories
SDIR = ./src
IDIR = ./include
LDIR = ./lib
ODIR = ./obj

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -I$(IDIR) 
SANITISE = -fsanitize=address -g -O1

MKDIR = mkdir -p
CP = cp -r -n

# Libraries
LIB := libgumbo/src
LIBDIR := $(LDIR)/$(LIB)
LIBO := attribute.o char_ref.o error.o parser.o string_buffer.o string_piece.o tag.o tokenizer.o utf8.o \
				util.o vector.o
LIBI := attribute.h char_ref.h error.h parser.h string_buffer.h string_piece.h tag_enum.h tag_gperf.h \
				tag_sizes.h tag_strings.h gumbo.h insertion_mode.h token_type.h tokenizer_states.h tokenizer.h \
				utf8.h util.h vector.h
LIBI := $(LIBH:%=$(LDIR)/%)

# Objects
OBJ := $(LIBO) crawler.o crawl.o http.o url.o client.o
OBJ := $(OBJ:%=$(ODIR)/%)
LIBO := $(LIBO:%=$(ODIR)/%)

#-L$(LDIR)  -Wl,-rpath,$(LDIR)

# Output
EXE = crawler

# Runtime args
# Used for debugging
TEST1 = http://ibdhost.com/help/html/
TEST2 = google.com
TEST3 = https://webhook.site/a6e635ec-e82e-4ef8-b94c-20820b1d823e

# Look in lib for extra header files
vpath %.h $(LIBDIR)

.PHONY: all run clean libgumbo

all: clean libgumbo $(EXE)

test1: all
	@./$(EXE) $(TEST1)

test2: all
	@./$(EXE) $(TEST2)

test3: all
	@./$(EXE) $(TEST3)

clean:
	@rm -r -f $(ODIR) $(EXE)

# Include libgumbo in build
libgumbo: $(LIBI) $(LIBO)

# Build src files
$(ODIR)/%.o: $(SDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< $(CFLAGS)

# Build lib src files
$(ODIR)/%.o: $(LIBDIR)/%.c
	@$(MKDIR) $(@D)
	@$(CC) -c -o $@ $< -I$(IDIR)

# Include all necessary header files
$(IDIR)/%: %
	@$(CP) $^ $(IDIR)

$(EXE): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)
