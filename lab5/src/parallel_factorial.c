#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/time.h>

// Глобальные переменные
int k = -1;           // число, факториал которого вычисляем
int pnum = -1;        // количество потоков
int mod = -1;         // модуль
long long result = 1; // общий результат (факториал)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // мьютекс для синхронизации

// Структура для передачи данных в поток
typedef struct {
    int thread_id;
    int start;
    int end;
    long long partial_result;
} ThreadData;

// Функция для вычисления частичного факториала
void* compute_partial_factorial(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long partial = 1;
    
    // Вычисляем произведение чисел от start до end
    for (int i = data->start; i <= data->end; i++) {
        partial = (partial * i) % mod;
    }
    
    data->partial_result = partial;
    
    // Критическая секция: умножаем общий результат на частичный
    pthread_mutex_lock(&mutex);
    result = (result * partial) % mod;
    pthread_mutex_unlock(&mutex);
    
    printf("Thread %d: computed product from %d to %d, partial = %lld\n", 
           data->thread_id, data->start, data->end, partial);
    
    return NULL;
}

int main(int argc, char** argv) {
    // Разбор аргументов командной строки
    while (1) {
        static struct option options[] = {
            {"k", required_argument, 0, 'k'},
            {"pnum", required_argument, 0, 'p'},
            {"mod", required_argument, 0, 'm'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        int c = getopt_long(argc, argv, "k:p:m:", options, &option_index);
        
        if (c == -1) break;
        
        switch (c) {
            case 'k':
                k = atoi(optarg);
                if (k < 0) {
                    printf("k must be >= 0\n");
                    return 1;
                }
                break;
            case 'p':
                pnum = atoi(optarg);
                if (pnum <= 0) {
                    printf("pnum must be positive\n");
                    return 1;
                }
                break;
            case 'm':
                mod = atoi(optarg);
                if (mod <= 0) {
                    printf("mod must be positive\n");
                    return 1;
                }
                break;
            default:
                printf("Unknown option\n");
                return 1;
        }
    }
    
    // Проверка обязательных аргументов
    if (k == -1 || pnum == -1 || mod == -1) {
        printf("Usage: %s -k <num> -p <num> -m <num>\n", argv[0]);
        printf("Example: %s -k 10 --pnum 4 --mod 10\n", argv[0]);
        printf("\nWhere:\n");
        printf("  -k, --k       number to compute factorial of\n");
        printf("  -p, --pnum    number of threads\n");
        printf("  -m, --mod     modulus for factorial\n");
        return 1;
    }
    
    printf("\n========================================\n");
    printf("Parallel Factorial Calculator\n");
    printf("========================================\n");
    printf("k = %d\n", k);
    printf("pnum = %d\n", pnum);
    printf("mod = %d\n", mod);
    printf("========================================\n\n");
    
    // Особый случай: 0! = 1
    if (k == 0) {
        printf("Result: %d! mod %d = 1\n", k, mod);
        return 0;
    }
    
    // Если потоков больше, чем чисел - корректируем
    if (pnum > k) {
        pnum = k;
        printf("Adjusted threads to %d (cannot have more threads than numbers)\n\n", pnum);
    }
    
    // Разбиваем диапазон [1..k] на pnum частей
    ThreadData* threads_data = (ThreadData*)malloc(pnum * sizeof(ThreadData));
    pthread_t* threads = (pthread_t*)malloc(pnum * sizeof(pthread_t));
    
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    struct timeval start_time, finish_time;
    gettimeofday(&start_time, NULL);
    
    // Создаем потоки
    for (int i = 0; i < pnum; i++) {
        threads_data[i].thread_id = i;
        threads_data[i].start = current_start;
        
        // Распределяем остаток: первые 'remainder' потоков получают на одно число больше
        int extra = (i < remainder) ? 1 : 0;
        threads_data[i].end = current_start + numbers_per_thread + extra - 1;
        threads_data[i].partial_result = 1;
        
        current_start = threads_data[i].end + 1;
        
        // Вывод информации о распределении
        printf("Thread %d: range [%d, %d]\n", i, threads_data[i].start, threads_data[i].end);
        
        if (pthread_create(&threads[i], NULL, compute_partial_factorial, &threads_data[i]) != 0) {
            perror("pthread_create failed");
            free(threads_data);
            free(threads);
            return 1;
        }
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&finish_time, NULL);
    
    // Вычисляем время выполнения
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    
    // Выводим результат
    printf("\n========================================\n");
    printf("RESULT:\n");
    printf("%d! mod %d = %lld\n", k, mod, result);
    printf("Elapsed time: %.3f ms\n", elapsed_time);
    printf("========================================\n");
    
    // Проверка: последовательное вычисление для верификации
    long long verification = 1;
    for (int i = 1; i <= k; i++) {
        verification = (verification * i) % mod;
    }
    printf("\nVerification: %d! mod %d = %lld\n", k, mod, verification);
    
    if (result == verification) {
        printf("✓ SUCCESS: Results match!\n");
    } else {
        printf("✗ ERROR: Results do not match!\n");
    }
    
    // Очистка
    free(threads_data);
    free(threads);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
