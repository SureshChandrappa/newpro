# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -z muldefs

# Target executables
TARGETS = combined_program main_sum

# Object files
COMBINED_OBJS = weakmain.o weaklibrary.o
MAIN_SUM_OBJS = main.o sum.o

# Default target
all: $(TARGETS)

# Link combined_program
combined_program: $(COMBINED_OBJS)
	$(CC) $(COMBINED_OBJS) -o combined_program $(LDFLAGS)

# Link main_sum
main_sum: $(MAIN_SUM_OBJS)
	$(CC) $(MAIN_SUM_OBJS) -o main_sum $(LDFLAGS)

# Compile weakmain.c
weakmain.o: weakmain.c
	$(CC) $(CFLAGS) -c weakmain.c -o weakmain.o

# Compile weaklibrary.c
weaklibrary.o: weaklibrary.c
	$(CC) $(CFLAGS) -c weaklibrary.c -o weaklibrary.o

# Compile main.c
main.o: main.c sum.h
	$(CC) $(CFLAGS) -c main.c -o main.o

# Compile sum.c
sum.o: sum.c sum.h
	$(CC) $(CFLAGS) -c sum.c -o sum.o

# Clean up build files
clean:
	rm -f $(COMBINED_OBJS) $(MAIN_SUM_OBJS) $(TARGETS)

# Phony targets
.PHONY: all clean
