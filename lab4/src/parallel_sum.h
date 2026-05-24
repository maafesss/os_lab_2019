#ifndef PARALLEL_SUM_H
#define PARALLEL_SUM_H

#include <stddef.h>

// Структура для передачи данных в поток
typedef struct {
    int *array;        // указатель на массив
    int start;         // начальный индекс
    int end;           // конечный индекс (включительно)
    long long sum;     // результат суммы
} ThreadData;

// Функция для подсчета суммы в части массива
void *ComputeSumPart(void *arg);

// Основная функция параллельного подсчета суммы
long long ParallelSum(int *array, int array_size, int num_threads);

#endif
