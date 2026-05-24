#include "common.h"

// Функция вычисления произведения чисел от start до end по модулю mod
long long compute_range(int start, int end, int mod) {
    long long result = 1;
    for (int i = start; i <= end; i++) {
        result = (result * i) % mod;
    }
    return result;
}

void handle_client(int client_fd) {
    Task task;
    Response response;
    
    // Получаем задание от клиента
    ssize_t bytes = recv(client_fd, &task, sizeof(Task), 0);
    if (bytes <= 0) {
        perror("recv failed");
        response.status = -1;
        send(client_fd, &response, sizeof(Response), 0);
        close(client_fd);
        return;
    }
    
    printf("Received task: start=%d, end=%d, mod=%d\n", 
           task.start, task.end, task.mod);
    
    // Вычисляем результат
    response.result = compute_range(task.start, task.end, task.mod);
    response.status = 0;
    
    printf("Computed result: %lld\n", response.result);
    
    // Отправляем результат клиенту
    send(client_fd, &response, sizeof(Response), 0);
    
    close(client_fd);
}

int main(int argc, char *argv[]) {
    int port = PORT;
    
    // Аргументы командной строки: ./server [port]
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(1);
    }
    
    // Настраиваем опцию SO_REUSEADDR для переиспользования порта
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(1);
    }
    
    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Принимаем соединения с любого IP
    server_addr.sin_port = htons(port);
    
    // Привязываем сокет к адресу
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    
    // Начинаем слушать соединения
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(1);
    }
    
    printf("Server started on port %d\n", port);
    printf("Waiting for connections...\n");
    
    // Основной цикл сервера
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        
        printf("Client connected from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        handle_client(client_fd);
        printf("Client disconnected\n\n");
    }
    
    close(server_fd);
    return 0;
}
