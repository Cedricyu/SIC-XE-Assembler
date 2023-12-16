CC = gcc
CFLAGS = -Wall -std=c11
SOURCES = main.c sym_node.c asm.c opcode.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = asm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c op.h sym_node.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
