CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -pthread

# Объектные файлы библиотеки
LIB_OBJ = libnetwork.o

all: server client

# Библиотека
libnetwork.o: libnetwork.c libnetwork.h
	$(CC) $(CFLAGS) -c libnetwork.c -o libnetwork.o

# Сервер
server: server.c $(LIB_OBJ)
	$(CC) $(CFLAGS) server.c $(LIB_OBJ) -o server $(LDFLAGS)

# Клиент
client: client.c $(LIB_OBJ)
	$(CC) $(CFLAGS) client.c $(LIB_OBJ) -o client $(LDFLAGS)

# Очистка
clean:
	rm -f server client *.o

# Полная пересборка
rebuild: clean all

# Запуск серверов
run-server1: server
	./server 8080

run-server2: server
	./server 8081

run-server3: server
	./server 8082

run-server4: server
	./server 8083

run-all-servers: server
	@echo "Starting 4 servers..."
	@./server 8080 &
	@./server 8081 &
	@./server 8082 &
	@./server 8083 &
	@echo "All servers started"

stop-servers:
	@-pkill -f "server"
	@echo "Servers stopped"

# Запуск клиента
run-client: client
	./client --k 20 --mod 1000 --servers servers.txt

test: client
	@echo "=== TEST 1: k=10, mod=100 ==="
	./client --k 10 --mod 100 --servers servers.txt
	@echo ""
	@echo "=== TEST 2: k=15, mod=200 ==="
	./client --k 15 --mod 200 --servers servers.txt
	@echo ""
	@echo "=== TEST 3: k=20, mod=1000 ==="
	./client --k 20 --mod 1000 --servers servers.txt

integration-test: server client
	@make run-all-servers
	@sleep 2
	@make test
	@make stop-servers

help:
	@echo "Commands:"
	@echo "  make all            - build everything"
	@echo "  make clean          - remove built files"
	@echo "  make server         - build server"
	@echo "  make client         - build client"
	@echo "  make run-server1    - run server on port 8080"
	@echo "  make run-all-servers - run all 4 servers"
	@echo "  make stop-servers   - stop all servers"
	@echo "  make run-client     - run client"
	@echo "  make test           - run tests"
	@echo "  make integration-test - auto test"

.PHONY: all clean rebuild server client \
        run-server1 run-server2 run-server3 run-server4 \
        run-all-servers stop-servers \
        run-client test integration-test help