#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>

#include "parallel_sum.h"
#include "utils.h"

int main(int argc, char **argv) {
    int threads_num = -1;
    int seed = -1;
    int array_size = -1;
    
    // Разбор аргументов командной строки
    while (true) {
        static struct option options[] = {
            {"threads_num", required_argument, 0, 0},
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);
        
        if (c == -1) break;
        
        if (c == 0) {
            switch (option_index) {
                case 0:
                    threads_num = atoi(optarg);
                    if (threads_num <= 0) {
                        printf("threads_num must be positive\n");
                        return 1;
                    }
                    break;
                case 1:
                    seed = atoi(optarg);
                    if (seed <= 0) {
                        printf("seed must be positive\n");
                        return 1;
                    }
                    break;
                case 2:
                    array_size = atoi(optarg);
                    if (array_size <= 0) {
                        printf("array_size must be positive\n");
                        return 1;
                    }
                    break;
            }
        }
    }
    
    // Проверка обязательных аргументов
    if (threads_num == -1 || seed == -1 || array_size == -1) {
        printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
        printf("Example: %s --threads_num 4 --seed 123 --array_size 1000000\n", argv[0]);
        return 1;
    }
    
    // Выделяем память под массив
    int *array = (int*)malloc(array_size * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Генерируем массив (НЕ попадает в замер времени)
    printf("Generating array with seed=%d, size=%d...\n", seed, array_size);
    GenerateArray(array, array_size, seed);
    
    // Замер времени только для подсчета суммы
    struct timeval start_time, finish_time;
    gettimeofday(&start_time, NULL);
    
    // Параллельный подсчет суммы
    long long total_sum = ParallelSum(array, array_size, threads_num);
    
    gettimeofday(&finish_time, NULL);
    
    // Вычисляем время в миллисекундах
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    
    // Выводим результат
    printf("\n=== RESULTS ===\n");
    printf("Array size: %d\n", array_size);
    printf("Number of threads: %d\n", threads_num);
    printf("Total sum: %lld\n", total_sum);
    printf("Elapsed time: %.3f ms\n", elapsed_time);
    
    // Проверка: считаем сумму последовательно для верификации
    long long sequential_sum = 0;
    for (int i = 0; i < array_size; i++) {
        sequential_sum += array[i];
    }
    printf("\nVerification:\n");
    printf("Parallel sum: %lld\n", total_sum);
    printf("Sequential sum: %lld\n", sequential_sum);
    
    if (total_sum == sequential_sum) {
        printf("✓ SUCCESS: Sums match!\n");
    } else {
        printf("✗ ERROR: Sums do not match!\n");
    }
    
    free(array);
    return 0;
}
