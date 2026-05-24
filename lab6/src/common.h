#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define PORT 8080

// Структура для передачи задания серверу
typedef struct {
    int start;      // начало диапазона
    int end;        // конец диапазона
    int mod;        // модуль
} Task;

// Структура для получения результата от сервера
typedef struct {
    long long result;  // результат вычислений
    int status;        // статус (0 - успех, -1 - ошибка)
} Response;

#endif
