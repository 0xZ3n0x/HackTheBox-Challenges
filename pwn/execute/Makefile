# Makefile

# Variables
ASM = payload.asm
OBJ = payload.o
TEXT_BIN = payload.bin
NASM = nasm
OBJCOPY = objcopy
PYTHON_FILE = exploit.py

# Default target
all: $(TEXT_BIN)

# .asm -> .o
$(OBJ): $(ASM)
	$(NASM) -f elf64 $(ASM) -o $(OBJ)

# .o -> only the .text section
$(TEXT_BIN): $(OBJ)
	$(OBJCOPY) -O binary --only-section=.text $(OBJ) $(TEXT_BIN)

# Clean build files
clean:
	rm -f $(OBJ) $(TEXT_BIN)

# Run target: first build, then run the Python script
run: all
	python3 $(PYTHON_FILE)
