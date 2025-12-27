CC = gcc
# -fPIC is required for creating Shared Objects (.so)
CFLAGS = -Wall -O2 -fPIC
LDFLAGS = -lpci

# Library Names
LIB_NAME = libeasyparallel.so
HEADER_NAME = easy_parallel.h

# Installation Paths (Standard Linux)
PREFIX ?= /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# Default target
all: $(LIB_NAME)

# Build the Shared Library
$(LIB_NAME): easy_parallel.o
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# Compile Object File
easy_parallel.o: easy_parallel.c easy_parallel.h
	$(CC) $(CFLAGS) -c easy_parallel.c -o easy_parallel.o

# Install Target (Needs sudo)
install: $(LIB_NAME)
	@echo "Installing EasyParallel..."
	@mkdir -p $(LIBDIR)
	@mkdir -p $(INCLUDEDIR)
	cp $(LIB_NAME) $(LIBDIR)/
	cp $(HEADER_NAME) $(INCLUDEDIR)/
	@chmod 755 $(LIBDIR)/$(LIB_NAME)
	@chmod 644 $(INCLUDEDIR)/$(HEADER_NAME)
	@ldconfig
	@echo "Installation Complete! You can now use <easy_parallel.h>"

# Uninstall Target
uninstall:
	@echo "Removing EasyParallel..."
	rm -f $(LIBDIR)/$(LIB_NAME)
	rm -f $(INCLUDEDIR)/$(HEADER_NAME)
	@ldconfig
	@echo "Uninstalled."

clean:
	rm -f *.o *.so

.PHONY: all install uninstall clean