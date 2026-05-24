#include "parallel_sum.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

// Функция, которую выполняет каждый поток
void *ComputeSumPart(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    long long sum = 0;
    
    for (int i = data->start; i <= data->end; i++) {
        sum += data->array[i];
    }
    
    data->sum = sum;
    return NULL;
}

// Основная функция: разбивает массив на части и запускает потоки
long long ParallelSum(int *array, int array_size, int num_threads) {
    if (num_threads <= 0) num_threads = 1;
    if (array_size <= 0) return 0;
    
    // Корректируем количество потоков, чтобы не было больше размера массива
    if (num_threads > array_size) {
        num_threads = array_size;
    }
    
    // Выделяем память под данные для потоков
    ThreadData *threads_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    pthread_t *threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    
    // Разбиваем массив на части
    int part_size = array_size / num_threads;
    int remainder = array_size % num_threads;
    
    int current_start = 0;
    
    // Создаем потоки
    for (int i = 0; i < num_threads; i++) {
        threads_data[i].array = array;
        threads_data[i].start = current_start;
        
        // Распределяем остаток
        int extra = (i < remainder) ? 1 : 0;
        threads_data[i].end = current_start + part_size + extra - 1;
        threads_data[i].sum = 0;
        
        current_start = threads_data[i].end + 1;
        
        // Создаем поток
        if (pthread_create(&threads[i], NULL, ComputeSumPart, &threads_data[i]) != 0) {
            perror("pthread_create failed");
            free(threads_data);
            free(threads);
            return -1;
        }
    }
    
    // Ждем завершения всех потоков и суммируем результаты
    long long total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += threads_data[i].sum;
    }
    
    // Очищаем память
    free(threads_data);
    free(threads);
    
    return total_sum;
}
