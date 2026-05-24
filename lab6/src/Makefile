
CC = gcc
CFLAGS = -Wall -pthread
TARGET = factorial_parallel
SRC = factorial_parallel.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) -k 10 -p 4 -m 100