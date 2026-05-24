#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int port = 9090;
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket failed");
        exit(1);
    }
    
    // Настройка адреса
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Привязка
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    
    printf("UDP Server started on port %d\n", port);
    
    while (1) {
        // Получаем сообщение от клиента
        int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr*)&client_addr, &client_len);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Received from %s:%d: %s\n", 
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
            
            // Отправляем ответ
            char response[] = "Hello from UDP server!";
            sendto(sock, response, strlen(response), 0,
                   (struct sockaddr*)&client_addr, client_len);
        }
    }
    
    close(sock);
    return 0;
}
