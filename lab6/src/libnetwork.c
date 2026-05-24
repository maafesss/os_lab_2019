#include "libnetwork.h"

// Функция вычисления произведения чисел от start до end по модулю mod
long long compute_range(int start, int end, int mod) {
    long long result = 1;
    for (int i = start; i <= end; i++) {
        result = (result * (i % mod)) % mod;
    }
    return result;
}

// Создание сокета
int create_socket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return -1;
    }
    return sock;
}

// Отправка задачи
int send_task(int sock_fd, Task *task) {
    ssize_t bytes = send(sock_fd, task, sizeof(Task), 0);
    if (bytes < 0) {
        perror("send task failed");
        return -1;
    }
    if (bytes != sizeof(Task)) {
        fprintf(stderr, "send task: incomplete send\n");
        return -1;
    }
    return 0;
}

// Получение результата
int receive_response(int sock_fd, Response *response) {
    ssize_t bytes = recv(sock_fd, response, sizeof(Response), 0);
    if (bytes < 0) {
        perror("recv response failed");
        return -1;
    }
    if (bytes != sizeof(Response)) {
        fprintf(stderr, "recv response: incomplete receive\n");
        return -1;
    }
    return 0;
}

// Получение задачи (для сервера)
int receive_task(int sock_fd, Task *task) {
    ssize_t bytes = recv(sock_fd, task, sizeof(Task), 0);
    if (bytes < 0) {
        perror("recv task failed");
        return -1;
    }
    if (bytes == 0) {
        return 0; // соединение закрыто
    }
    if (bytes != sizeof(Task)) {
        fprintf(stderr, "recv task: incomplete receive\n");
        return -1;
    }
    return 1; // успешно получили задачу
}

// Отправка результата (для сервера)
int send_response(int sock_fd, Response *response) {
    ssize_t bytes = send(sock_fd, response, sizeof(Response), 0);
    if (bytes < 0) {
        perror("send response failed");
        return -1;
    }
    if (bytes != sizeof(Response)) {
        fprintf(stderr, "send response: incomplete send\n");
        return -1;
    }
    return 0;
}

// Настройка серверного сокета
int setup_server_socket(int port) {
    int server_fd;
    struct sockaddr_in server_addr;
    
    // Создаем сокет
    server_fd = create_socket();
    if (server_fd < 0) {
        return -1;
    }
    
    // Настраиваем опцию SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }
    
    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Привязываем сокет
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }
    
    // Начинаем слушать
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }
    
    return server_fd;
}
