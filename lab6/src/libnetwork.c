#include "libnetwork.h"

long long compute_range(int start, int end, int mod) {
    long long result = 1;
    for (int i = start; i <= end; i++) {
        result = (result * (i % mod)) % mod;
    }
    return result;
}

int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return -1;
    }
    return sock;
}
// Подключение к серверу (для клиента)
int connect_to_server(const char *ip, int port) {
    int sock = create_socket();
    if (sock < 0) return -1;
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sock);
        return -1;
    }
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sock);
        return -1;
    }
    
    return sock;
}

int send_task(int sock_fd, Task *task) {
    ssize_t bytes = send(sock_fd, task, sizeof(Task), 0);
    if (bytes != sizeof(Task)) {
        perror("send task failed");
        return -1;
    }
    return 0;
}

int receive_response(int sock_fd, Response *response) {
    ssize_t bytes = recv(sock_fd, response, sizeof(Response), 0);
    if (bytes != sizeof(Response)) {
        perror("recv response failed");
        return -1;
    }
    return 0;
}

int receive_task(int sock_fd, Task *task) {
    ssize_t bytes = recv(sock_fd, task, sizeof(Task), 0);
    if (bytes != sizeof(Task)) {
        if (bytes == 0) return 0;
        perror("recv task failed");
        return -1;
    }
    return 1;
}

int send_response(int sock_fd, Response *response) {
    ssize_t bytes = send(sock_fd, response, sizeof(Response), 0);
    if (bytes != sizeof(Response)) {
        perror("send response failed");
        return -1;
    }
    return 0;
}

int setup_server_socket(int port) {
    int server_fd = create_socket();
    if (server_fd < 0) return -1;
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }
    
    return server_fd;
}