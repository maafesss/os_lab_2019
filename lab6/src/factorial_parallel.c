#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

// Структура для передачи аргументов в потоки
typedef struct {
    int thread_id;          // ID потока
    int start;              // Начальное значение (включительно)
    int end;                // Конечное значение (не включая)
    int mod;                // Модуль
    long long *result;      // Указатель на общий результат
    pthread_mutex_t *mutex; // Указатель на мьютекс
} ThreadArgs;

// Функция потока для вычисления части факториала
void* compute_partial_factorial(void* arg) {
    ThreadArgs *args = (ThreadArgs*)arg;
    long long partial_result = 1;
    
    // Вычисляем частичное произведение для диапазона [start, end)
    for (int i = args->start; i <= args->end; i++) {
        partial_result = (partial_result * i) % args->mod;
    }
    
    // Блокируем мьютекс для обновления общего результата
    pthread_mutex_lock(args->mutex);
    *(args->result) = (*(args->result) * partial_result) % args->mod;
    pthread_mutex_unlock(args->mutex);
    
    printf("Поток %d: вычислил [%d..%d] = %lld\n", 
           args->thread_id, args->start, args->end, partial_result);
    
    return NULL;
}

int main(int argc, char *argv[]) {
    int k = 0, pnum = 0, mod = 0;
    int opt;
    
    // Разбор аргументов командной строки
    while ((opt = getopt(argc, argv, "k:p:m:")) != -1) {
        switch (opt) {
            case 'k': k = atoi(optarg); break;
            case 'p': pnum = atoi(optarg); break;
            case 'm': mod = atoi(optarg); break;
            default:
                fprintf(stderr, "Использование: %s -k <число> -p <потоков> -m <модуль>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Ошибка: все параметры должны быть > 0\n");
        exit(EXIT_FAILURE);
    }
    
    if (pnum > k) pnum = k;
    
    printf("Вычисление %d! mod %d, потоков: %d\n", k, mod, pnum);
    
    pthread_t *threads = malloc(pnum * sizeof(pthread_t));
    ThreadArgs *args = malloc(pnum * sizeof(ThreadArgs));
    long long result = 1;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    
    // Распределение чисел между потоками
    int step = k / pnum;
    int remainder = k % pnum;
    int current = 1;
    
    for (int i = 0; i < pnum; i++) {
        int chunk_size = step + (i < remainder ? 1 : 0);
        
        args[i].thread_id = i;
        args[i].start = current;
        args[i].end = current + chunk_size - 1;
        args[i].mod = mod;
        args[i].result = &result;
        args[i].mutex = &mutex;
        
        pthread_create(&threads[i], NULL, compute_partial_factorial, &args[i]);
        current += chunk_size;
    }
    
    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\nРезультат: %d! mod %d = %lld\n", k, mod, result);
    
    pthread_mutex_destroy(&mutex);
    free(threads);
    free(args);
    
    return 0;
}
