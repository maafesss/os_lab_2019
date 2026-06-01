#include "libnetwork.h"

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc > 1) port = atoi(argv[1]);
    
    int server_fd = setup_server_socket(port);
    if (server_fd < 0) exit(1);
    
    printf("[SERVER] Started on port %d\n", port);
    
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
        
        Task task;
        if (receive_task(client_fd, &task) > 0) {
            printf("[SERVER] Task: [%d, %d] mod %d\n", task.start, task.end, task.mod);
            
            Response resp;
            resp.result = compute_range(task.start, task.end, task.mod);
            resp.status = 0;
            
            send_response(client_fd, &resp);
            printf("[SERVER] Result: %lld sent\n", resp.result);
        }
        
        close(client_fd);
        printf("[SERVER] Client disconnected\n");
    }
    
    close(server_fd);
    return 0;
}