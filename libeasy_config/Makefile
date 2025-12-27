CC = gcc
# -fPIC is required for shared libraries (Position Independent Code)
CFLAGS = -Wall -Wextra -O2 -fPIC
LDFLAGS = -shared
TARGET_LIB = libeasyconfig.so
HEADER = easy_config.h
SOURCES = easy_config.c
OBJECTS = $(SOURCES:.c=.o)

# Standard installation paths
PREFIX ?= /usr/local
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

.PHONY: all clean install uninstall test

all: $(TARGET_LIB)

# Link the object files into a shared library
$(TARGET_LIB): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Install the library and header
install: all
	@echo "Installing to $(PREFIX)..."
	@mkdir -p $(LIBDIR)
	@mkdir -p $(INCDIR)
	install -m 755 $(TARGET_LIB) $(LIBDIR)
	install -m 644 $(HEADER) $(INCDIR)
	@echo "Updating shared library cache..."
	ldconfig
	@echo "Installation successful!"

# Remove installed files
uninstall:
	rm -f $(LIBDIR)/$(TARGET_LIB)
	rm -f $(INCDIR)/$(HEADER)
	ldconfig
	@echo "Uninstallation successful!"

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET_LIB) test_app

# --- Optional: Build the test app to verify installation ---
# Run this AFTER 'sudo make install'
test_app: test_main.c
	$(CC) -o test_app test_main.c -leasyconfig

test: test_app
	./test_app