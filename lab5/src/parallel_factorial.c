#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <string.h>

// Глобальные переменные
long long factorial_result = 1;
pthread_mutex_t result_mutex;

// Структура для передачи параметров потоку
typedef struct {
    int start;
    int end;
    int mod;
} ThreadArgs;

// Функция, которую выполняет каждый поток
void* compute_partial_factorial(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    long long local_result = 1;
    
    // Вычисляем частичное произведение для своего диапазона
    for (int i = args->start; i <= args->end; i++) {
        local_result = (local_result * i) % args->mod;
    }
    
    // Блокируем мьютекс перед обновлением глобального результата
    pthread_mutex_lock(&result_mutex);
    factorial_result = (factorial_result * local_result) % args->mod;
    pthread_mutex_unlock(&result_mutex);
    
    free(args);
    return NULL;
}

int main(int argc, char* argv[]) {
    int k = 0;
    int pnum = 1;
    int mod = 1;
    
    // Парсинг аргументов командной строки
    static struct option long_options[] = {
        {"k", required_argument, 0, 'k'},
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "k:p:m:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Использование: %s -k <число> -p <количество потоков> -m <модуль>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    // Проверка корректности входных параметров
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Ошибка: все параметры должны быть положительными числами\n");
        fprintf(stderr, "Использование: %s -k <число> -p <количество потоков> -m <модуль>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (pnum > k) {
        pnum = k;
        printf("Предупреждение: потоков больше чем чисел, уменьшено до %d\n", pnum);
    }
    
    // Инициализация мьютекса
    if (pthread_mutex_init(&result_mutex, NULL) != 0) {
        fprintf(stderr, "Ошибка инициализации мьютекса\n");
        exit(EXIT_FAILURE);
    }
    
    // Массив для хранения идентификаторов потоков
    pthread_t threads[pnum];
    
    // Определяем диапазоны для каждого потока
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    // Создаём потоки
    for (int i = 0; i < pnum; i++) {
        int current_end = current_start + numbers_per_thread - 1;
        if (i < remainder) {
            current_end++;
        }
        
        // Не выходим за пределы k
        if (current_end > k) {
            current_end = k;
        }
        
        // Выделяем память для аргументов потока
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(EXIT_FAILURE);
        }
        
        args->start = current_start;
        args->end = current_end;
        args->mod = mod;
        
        // Создаём поток
        if (pthread_create(&threads[i], NULL, compute_partial_factorial, (void*)args) != 0) {
            fprintf(stderr, "Ошибка создания потока %d\n", i);
            exit(EXIT_FAILURE);
        }
        
        current_start = current_end + 1;
    }
    
    // Ожидаем завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Выводим результат
    printf("%d! mod %d = %lld\n", k, mod, factorial_result);
    
    // Уничтожаем мьютекс
    pthread_mutex_destroy(&result_mutex);
    
    return 0;
}