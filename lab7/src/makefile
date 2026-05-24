CC = gcc
CFLAGS = -Wall -Wextra -g

all: tcp_server tcp_client udp_server udp_client

tcp_server: tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c -o tcp_server

tcp_client: tcp_client.c
	$(CC) $(CFLAGS) tcp_client.c -o tcp_client

udp_server: udp_server.c
	$(CC) $(CFLAGS) udp_server.c -o udp_server

udp_client: udp_client.c
	$(CC) $(CFLAGS) udp_client.c -o udp_client

clean:
	rm -f tcp_server tcp_client udp_server udp_client

rebuild: clean all

# Запуск TCP сервера (по умолчанию порт 8080)
run-tcp-server:
	./tcp_server $(PORT)

# Запуск TCP клиента (по умолчанию 127.0.0.1 8080)
run-tcp-client:
	./tcp_client $(IP) $(PORT)

# Запуск UDP сервера (по умолчанию порт 9090)
run-udp-server:
	./udp_server $(PORT)

# Запуск UDP клиента (по умолчанию 127.0.0.1 9090)
run-udp-client:
	./udp_client $(IP) $(PORT)

# Демонстрация TCP
demo-tcp:
	@echo "=== TCP Demo ==="
	@echo "Terminal 1: make run-tcp-server PORT=8080"
	@echo "Terminal 2: make run-tcp-client IP=127.0.0.1 PORT=8080"

# Демонстрация UDP
demo-udp:
	@echo "=== UDP Demo ==="
	@echo "Terminal 1: make run-udp-server PORT=9090"
	@echo "Terminal 2: make run-udp-client IP=127.0.0.1 PORT=9090"

help:
	@echo "========================================"
	@echo "LAB 7 - TCP/UDP Client-Server"
	@echo "========================================"
	@echo ""
	@echo "BUILD:"
	@echo "  make all           - build all programs"
	@echo "  make clean         - remove built files"
	@echo ""
	@echo "RUN TCP:"
	@echo "  make run-tcp-server PORT=8080"
	@echo "  make run-tcp-client IP=127.0.0.1 PORT=8080"
	@echo ""
	@echo "RUN UDP:"
	@echo "  make run-udp-server PORT=9090"
	@echo "  make run-udp-client IP=127.0.0.1 PORT=9090"
	@echo ""
	@echo "EXAMPLE:"
	@echo "  Terminal 1: make run-tcp-server PORT=8080"
	@echo "  Terminal 2: make run-tcp-client PORT=8080"
	@echo ""

.PHONY: all clean rebuild run-tcp-server run-tcp-client run-udp-server run-udp-client demo-tcp demo-udp help
# Освобождение портов
kill-tcp:
	-pkill -9 tcp_server
	@echo "TCP servers killed"

kill-udp:
	-pkill -9 udp_server
	@echo "UDP servers killed"

kill-all: kill-tcp kill-udp
	@echo "All servers killed"

# Запуск с автоматическим убийством предыдущего
restart-tcp-server: kill-tcp
	./tcp_server $(PORT)

restart-udp-server: kill-udp
	./udp_server $(PORT)
