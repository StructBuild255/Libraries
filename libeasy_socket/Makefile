# Compiler and Flags
CC = gcc
# -fPIC is required for shared libraries (Position Independent Code)
# -Wall -Wextra are standard warning flags for clean C code
CFLAGS = -Wall -Wextra -O2 -fPIC

# Project Name
LIB_NAME = libeasy_socket
SRC = easy_socket.c
OBJ = easy_socket.o

# Installation Paths (Standard Linux structure)
PREFIX = /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# Targets
.PHONY: all static shared clean install uninstall

all: static shared

# Compile the object file
$(OBJ): $(SRC) easy_socket.h
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Build Static Library (.a)
# Used if you want to bundle the library code inside your final executable
static: $(OBJ)
	ar rcs $(LIB_NAME).a $(OBJ)

# Build Shared Library (.so)
# Used if you want the library to exist separately (common for system-wide libs)
shared: $(OBJ)
	$(CC) -shared -o $(LIB_NAME).so $(OBJ)

# Install headers and libs to system directories
# (Likely requires sudo)
install: all
	@echo "Installing to $(PREFIX)..."
	install -d $(LIBDIR)
	install -d $(INCLUDEDIR)
	install -m 644 $(LIB_NAME).a $(LIBDIR)
	install -m 755 $(LIB_NAME).so $(LIBDIR)
	install -m 644 easy_socket.h $(INCLUDEDIR)
	@echo "Installation complete. You may need to run 'sudo ldconfig'."

# Remove installed files
uninstall:
	rm -f $(LIBDIR)/$(LIB_NAME).a
	rm -f $(LIBDIR)/$(LIB_NAME).so
	rm -f $(INCLUDEDIR)/easy_socket.h
	@echo "Uninstallation complete."

# Clean build artifacts
clean:
	rm -f *.o *.a *.so