CC = gcc
CFLAGS = -Wall -std=c11 -g
SOURCES = main.c sym_node.c asm.c opcode.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = asm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -f *.o

.PHONY: all clean
