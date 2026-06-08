# --- Compiler Settings ---
CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -I header

# --- Files and Folders ---
# This automatically finds every .c file in your source folder!
SRCS = $(wildcard source/*.c)

# This dynamically replaces the .c extension with .o for the object files
OBJS = $(SRCS:.c=.o)

# The name of your final program
TARGET = simulator

# --- Build Rules ---

# The default rule (what happens when you just type 'make')
all: $(TARGET)

# How to link the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# How to compile individual .c files into .o object files
source/%.o: source/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# A handy cleanup command to wipe out old generated files
clean:
	rm -f source/*.o $(TARGET)