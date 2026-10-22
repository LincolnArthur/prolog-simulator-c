# Compilação: digite "make" (Linux/macOS) ou "mingw32-make" (Windows com MinGW)
CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11 -O2
SRC     = $(wildcard src/*.c)
TARGET  = prolog_simulator

all: $(TARGET)

$(TARGET): $(SRC) $(wildcard src/*.h)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe src/*.o

.PHONY: all run clean
