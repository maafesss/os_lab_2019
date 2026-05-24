#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Глобальная переменная (разделяемый ресурс)
int counter = 0;

// Мьютекс для защиты критической секции
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Флаг: использовать мьютекс или нет (по умолчанию 0 - не использовать)
int use_mutex = 0;

// Функция, которую выполняют потоки
void* increment_counter(void* arg) {
    int thread_id = *(int*)arg;
    int iterations = 1000000;  // 1 миллион итераций на поток
    
    for (int i = 0; i < iterations; i++) {
        if (use_mutex) {
            pthread_mutex_lock(&mutex);   // НАЧАЛО критической секции
        }
        
        // КРИТИЧЕСКАЯ СЕКЦИЯ (доступ к общему ресурсу)
        counter++;  // counter = counter + 1
        
        if (use_mutex) {
            pthread_mutex_unlock(&mutex); // КОНЕЦ критической секции
        }
    }
    
    printf("Thread %d finished\n", thread_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    int num_threads = 4;
    
    // Проверяем аргумент командной строки для мьютекса
    if (argc > 1) {
        if (atoi(argv[1]) == 1) {
            use_mutex = 1;
            printf("Mutex: ON\n");
        } else {
            printf("Mutex: OFF\n");
        }
    } else {
        printf("Usage: %s <0|1> (0 - no mutex, 1 - with mutex)\n", argv[0]);
        printf("Example: %s 0  (race condition)\n", argv[0]);
        printf("         %s 1  (correct synchronization)\n\n", argv[0]);
    }
    
    printf("Starting %d threads, each doing 1,000,000 increments\n", num_threads);
    printf("Expected final counter: %d\n\n", num_threads * 1000000);
    
    // Массив для хранения ID потоков
    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    
    // Создаем потоки
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, increment_counter, &thread_ids[i]) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n========================================\n");
    printf("Final counter value: %d\n", counter);
    printf("Expected value: %d\n", num_threads * 1000000);
    
    if (counter == num_threads * 1000000) {
        printf("✓ CORRECT! No race condition.\n");
    } else {
        printf("✗ RACE CONDITION! Lost %d increments.\n", 
               num_threads * 1000000 - counter);
    }
    printf("========================================\n");
    
    // Уничтожаем мьютекс (хотя в данном случае необязательно)
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
