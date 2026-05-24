CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -pthread

# Объектные файлы библиотеки
LIB_OBJS = libnetwork.o

all: server client

# Библиотека
libnetwork.o: libnetwork.c libnetwork.h
	$(CC) $(CFLAGS) -c libnetwork.c -o libnetwork.o

# Сервер использует библиотеку
server: server.c libnetwork.o
	$(CC) $(CFLAGS) server.c libnetwork.o -o server $(LDFLAGS)

# Клиент использует библиотеку
client: client.c libnetwork.o
	$(CC) $(CFLAGS) client.c libnetwork.o -o client $(LDFLAGS)

clean:
	rm -f server client *.o

rebuild: clean all

run-server1: server
	./server 8080

run-server2: server
	./server 8081

run-server3: server
	./server 8082

run-server4: server
	./server 8083

run-client: client
	./client --k 20 --mod 1000 --servers servers.txt

test: client
	./client --k 10 --mod 100 --servers servers.txt
	./client --k 15 --mod 200 --servers servers.txt
	./client --k 20 --mod 1000 --servers servers.txt

help:
	@echo "Commands:"
	@echo "  make all       - build everything"
	@echo "  make clean     - remove built files"
	@echo "  make server    - build server"
	@echo "  make client    - build client"
	@echo "  make run-server1 - run server on port 8080"
	@echo "  make run-client  - run client"
	@echo "  make test      - run tests"

.PHONY: all clean rebuild server client run-server1 run-server2 run-server3 run-server4 run-client test help
