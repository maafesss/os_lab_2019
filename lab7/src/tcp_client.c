#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char *server_ip = "127.0.0.1";
    int port = 8080;
    
    if (argc > 1) server_ip = argv[1];
    if (argc > 2) port = atoi(argv[2]);
    
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(1);
    }
    
    // Настройка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(1);
    }
    
    // Подключение к серверу
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        exit(1);
    }
    
    printf("Connected to TCP server at %s:%d\n", server_ip, port);
    
    // Отправляем сообщение
    char message[] = "Hello from TCP client!";
    send(sock, message, strlen(message), 0);
    printf("Sent: %s\n", message);
    
    // Получаем ответ
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
    }
    
    close(sock);
    return 0;
}
