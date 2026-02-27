# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -z muldefs

# Target executable
TARGET = combined_program

# Object files
OBJS = weakmain.o weaklibrary.o

# Default target
all: $(TARGET)

# Link the program (using -z muldefs to allow multiple definitions)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile weakmain.c
weakmain.o: weakmain.c
	$(CC) $(CFLAGS) -c weakmain.c -o weakmain.o

# Compile weaklibrary.c
weaklibrary.o: weaklibrary.c
	$(CC) $(CFLAGS) -c weaklibrary.c -o weaklibrary.o

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
