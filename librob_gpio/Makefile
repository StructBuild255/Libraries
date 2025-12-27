CC = gcc
# -I. tells the compiler to look in the current folder for the .h file
CFLAGS = -I. -Wall -Wextra -O2

SRC = rob_gpio.c
OBJ = rob_gpio.o
OUT = librob_gpio.a

# Standard Linux locations
INSTALL_LIB_PATH = /usr/local/lib
INSTALL_INC_PATH = /usr/local/include

all: clean build

build:
	# 1. Compile source to object
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)
	
	# 2. Archive object into static lib
	ar rcs $(OUT) $(OBJ)
	
	# 3. Cleanup object file
	rm $(OBJ)
	@echo "Build Complete: $(OUT)"

install: build
	@echo "Installing to $(INSTALL_LIB_PATH)..."
	# Copy header
	cp rob_gpio.h $(INSTALL_INC_PATH)/
	# Copy library
	cp $(OUT) $(INSTALL_LIB_PATH)/
	@echo "Installation Complete."

clean:
	rm -f $(OBJ) $(OUT)