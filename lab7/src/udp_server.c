#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int port = 9090;
    if (argc > 1) port = atoi(argv[1]);
    
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket failed"); exit(1); }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed"); exit(1);
    }
    
    printf("UDP Server started on port %d\n", port);
    
    while (1) {
        int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                            (struct sockaddr*)&client_addr, &client_len);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Received from %s:%d: %s\n", 
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
            
            sendto(sock, "Hello from UDP server!", 22, 0,
                   (struct sockaddr*)&client_addr, client_len);
        }
    }
    
    close(sock);
    return 0;
}