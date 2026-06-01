#ifndef LIBNETWORK_H
#define LIBNETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 8080

typedef struct {
    int start;
    int end;
    int mod;
} Task;

typedef struct {
    long long result;
    int status;
} Response;

long long compute_range(int start, int end, int mod);
int create_socket();
int send_task(int sock_fd, Task *task);
int receive_response(int sock_fd, Response *response);
int receive_task(int sock_fd, Task *task);
int send_response(int sock_fd, Response *response);
int setup_server_socket(int port);

#endif
