#include "libnetwork.h"

void handle_client(int client_fd) {
    Task task;
    Response response;
    
    // Получаем задание от клиента
    if (receive_task(client_fd, &task) <= 0) {
        response.status = -1;
        send_response(client_fd, &response);
        close(client_fd);
        return;
    }
    
    printf("[SERVER] Received task: start=%d, end=%d, mod=%d\n", 
           task.start, task.end, task.mod);
    
    // Вычисляем результат
    response.result = compute_range(task.start, task.end, task.mod);
    response.status = 0;
    
    printf("[SERVER] Computed result: %lld\n", response.result);
    
    // Отправляем результат клиенту
    send_response(client_fd, &response);
    
    close(client_fd);
    printf("[SERVER] Client disconnected\n\n");
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    int server_fd = setup_server_socket(port);
    if (server_fd < 0) {
        exit(1);
    }
    
    printf("[SERVER] Started on port %d\n", port);
    printf("[SERVER] Waiting for connections...\n\n");
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        
        printf("[SERVER] Client connected from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        handle_client(client_fd);
    }
    
    close(server_fd);
    return 0;
}
