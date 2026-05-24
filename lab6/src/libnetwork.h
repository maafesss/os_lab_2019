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

// Структура для передачи задания серверу
typedef struct {
    int start;      // начало диапазона
    int end;        // конец диапазона
    int mod;        // модуль
} Task;

// Структура для получения результата от сервера
typedef struct {
    long long result;  // результат вычислений
    int status;        // 0 - успех, -1 - ошибка
} Response;

// Функция вычисления произведения чисел от start до end по модулю mod
long long compute_range(int start, int end, int mod);

// Функция создания сокета
int create_socket();

// Функция отправки задачи
int send_task(int sock_fd, Task *task);

// Функция получения результата
int receive_response(int sock_fd, Response *response);

// Функция получения задачи (для сервера)
int receive_task(int sock_fd, Task *task);

// Функция отправки результата (для сервера)
int send_response(int sock_fd, Response *response);

// Функция настройки серверного сокета
int setup_server_socket(int port);

#endif
