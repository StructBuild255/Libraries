# Compiler configuration
CC = gcc
CFLAGS = -Wall -Wextra -g

# --- PROJECT SETTINGS ---
LIB_NAME = libeasy_serial.a
HEADER_NAME = easy_serial.h

# Installation Directories (Standard Linux locations)
PREFIX ?= /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# --- SOURCES ---
LIB_SRCS = easy_serial.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

# Optional: Test Tool (only builds if you explicitly ask or have main.c)
EXEC_NAME = lora_tool
EXEC_SRCS = main.c
EXEC_OBJS = $(EXEC_SRCS:.c=.o)

# --- TARGETS ---

# Default: Just build the library (safest option)
all: $(LIB_NAME)

# 1. Build the Static Library
$(LIB_NAME): $(LIB_OBJS)
	ar rcs $@ $^
	@echo "----------------------------------------"
	@echo "Library $(LIB_NAME) built successfully."
	@echo "Run 'sudo make install' to install it."
	@echo "----------------------------------------"

# 2. Install library headers to system (Requires Sudo)
install: $(LIB_NAME)
	@echo "Installing to $(PREFIX)..."
	# Create directories if they don't exist
	install -d $(INCLUDEDIR)
	install -d $(LIBDIR)
	# Copy files
	install -m 644 $(HEADER_NAME) $(INCLUDEDIR)
	install -m 644 $(LIB_NAME) $(LIBDIR)
	@echo "----------------------------------------"
	@echo "Success! You can now use #include <$(HEADER_NAME)>"
	@echo "Compile with: gcc your_app.c -o app -l:$(LIB_NAME)"
	@echo "----------------------------------------"

# 3. Uninstall (Clean up system files)
uninstall:
	rm -f $(INCLUDEDIR)/$(HEADER_NAME)
	rm -f $(LIBDIR)/$(LIB_NAME)
	@echo "Uninstalled easy_serial from system."

# 4. Build the test tool (Optional)
tool: $(EXEC_NAME)

$(EXEC_NAME): $(EXEC_OBJS) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $(EXEC_OBJS) -L. -leasy_serial
	@echo "Tool built: $(EXEC_NAME)"

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup local build files
clean:
	rm -f *.o *.a $(EXEC_NAME)
	@echo "Cleaned up local build files."

.PHONY: all install uninstall tool clean