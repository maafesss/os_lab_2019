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

run-tcp-server:
	./tcp_server $(PORT)

run-tcp-client:
	./tcp_client $(IP) $(PORT)

run-udp-server:
	./udp_server $(PORT)

run-udp-client:
	./udp_client $(IP) $(PORT)

kill:
	-pkill -f "tcp_server\|udp_server"

help:
	@echo "Commands:"
	@echo "  make all           - build all"
	@echo "  make clean         - clean"
	@echo "  make run-tcp-server PORT=8080"
	@echo "  make run-tcp-client IP=127.0.0.1 PORT=8080"
	@echo "  make run-udp-server PORT=9090"
	@echo "  make run-udp-client IP=127.0.0.1 PORT=9090"

.PHONY: all clean rebuild run-tcp-server run-tcp-client run-udp-server run-udp-client kill help
