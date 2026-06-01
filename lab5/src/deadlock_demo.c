#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Два мьютекса для создания deadlock
pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;

// Счётчики для наглядности
int counterA = 0;
int counterB = 0;

// Функция для потока 1
// Захватывает mutexA, затем mutexB
void* thread1_func(void* arg) {
    printf("Поток 1: пытаюсь захватить mutexA\n");
    pthread_mutex_lock(&mutexA);
    printf("Поток 1: захватил mutexA\n");
    
    // Небольшая задержка, чтобы увеличить шанс deadlock
    usleep(1000);
    
    printf("Поток 1: пытаюсь захватить mutexB\n");
    pthread_mutex_lock(&mutexB);
    printf("Поток 1: захватил mutexB\n");
    
    // Критическая секция
    counterA++;
    counterB++;
    printf("Поток 1: выполнил работу (counterA=%d, counterB=%d)\n", counterA, counterB);
    
    // Освобождаем в обратном порядке
    pthread_mutex_unlock(&mutexB);
    pthread_mutex_unlock(&mutexA);
    
    printf("Поток 1: завершил работу\n");
    return NULL;
}

// Функция для потока 2
// Захватывает mutexB, затем mutexA (обратный порядок!)
void* thread2_func(void* arg) {
    printf("Поток 2: пытаюсь захватить mutexB\n");
    pthread_mutex_lock(&mutexB);
    printf("Поток 2: захватил mutexB\n");
    
    // Небольшая задержка, чтобы увеличить шанс deadlock
    usleep(1000);
    
    printf("Поток 2: пытаюсь захватить mutexA\n");
    pthread_mutex_lock(&mutexA);
    printf("Поток 2: захватил mutexA\n");
    
    // Критическая секция
    counterA++;
    counterB++;
    printf("Поток 2: выполнил работу (counterA=%d, counterB=%d)\n", counterA, counterB);
    
    // Освобождаем в обратном порядке
    pthread_mutex_unlock(&mutexA);
    pthread_mutex_unlock(&mutexB);
    
    printf("Поток 2: завершил работу\n");
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t thread1, thread2;
    
    printf("=== Демонстрация deadlock ===\n");
    printf("Поток 1 захватывает mutexA -> mutexB\n");
    printf("Поток 2 захватывает mutexB -> mutexA\n");
    printf("Если порядок захвата разный — возникает deadlock\n\n");
    
    // Создаём потоки
    if (pthread_create(&thread1, NULL, thread1_func, NULL) != 0) {
        perror("Ошибка создания потока 1");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_create(&thread2, NULL, thread2_func, NULL) != 0) {
        perror("Ошибка создания потока 2");
        exit(EXIT_FAILURE);
    }
    
    // Ждём завершения потоков (при deadlock программа зависнет)
    printf("Ожидание завершения потоков...\n");
    printf("(если возник deadlock, программа зависнет навсегда)\n");
    printf("Нажмите Ctrl+C для принудительного завершения\n\n");
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("\nОба потока успешно завершились!\n");
    printf("Deadlock не произошёл (редкий случай)\n");
    
    return 0;
}