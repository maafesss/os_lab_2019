#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Два мьютекса (ресурса)
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

// Флаг для предотвращения deadlock (для версии без deadlock)
int avoid_deadlock = 0;

// Поток 1: захватывает mutex A, потом mutex B
void* thread1_func(void* arg) {
    if (avoid_deadlock) {
        // Версия без deadlock - одинаковый порядок захвата
        printf("Thread 1: trying to lock mutex A...\n");
        pthread_mutex_lock(&mutex_a);
        printf("Thread 1: locked mutex A\n");
        sleep(1);
        
        printf("Thread 1: trying to lock mutex B...\n");
        pthread_mutex_lock(&mutex_b);
        printf("Thread 1: locked mutex B\n");
    } else {
        // Версия с deadlock - обратный порядок захвата
        printf("Thread 1: trying to lock mutex A...\n");
        pthread_mutex_lock(&mutex_a);
        printf("Thread 1: locked mutex A\n");
        sleep(1);
        
        printf("Thread 1: trying to lock mutex B...\n");
        pthread_mutex_lock(&mutex_b);
        printf("Thread 1: locked mutex B\n");
    }
    
    // Критическая секция
    printf("Thread 1: working in critical section...\n");
    sleep(1);
    
    // Освобождаем ресурсы
    pthread_mutex_unlock(&mutex_b);
    printf("Thread 1: unlocked mutex B\n");
    pthread_mutex_unlock(&mutex_a);
    printf("Thread 1: unlocked mutex A\n");
    
    return NULL;
}

// Поток 2: захватывает mutex B, потом mutex A
void* thread2_func(void* arg) {
    if (avoid_deadlock) {
        // Версия без deadlock - ТАКОЙ ЖЕ порядок захвата (сначала A, потом B)
        printf("Thread 2: trying to lock mutex A...\n");
        pthread_mutex_lock(&mutex_a);
        printf("Thread 2: locked mutex A\n");
        sleep(1);
        
        printf("Thread 2: trying to lock mutex B...\n");
        pthread_mutex_lock(&mutex_b);
        printf("Thread 2: locked mutex B\n");
    } else {
        // Версия с deadlock - обратный порядок захвата (сначала B, потом A)
        printf("Thread 2: trying to lock mutex B...\n");
        pthread_mutex_lock(&mutex_b);
        printf("Thread 2: locked mutex B\n");
        sleep(1);
        
        printf("Thread 2: trying to lock mutex A...\n");
        pthread_mutex_lock(&mutex_a);
        printf("Thread 2: locked mutex A\n");
    }
    
    // Критическая секция
    printf("Thread 2: working in critical section...\n");
    sleep(1);
    
    // Освобождаем ресурсы
    pthread_mutex_unlock(&mutex_a);
    printf("Thread 2: unlocked mutex A\n");
    pthread_mutex_unlock(&mutex_b);
    printf("Thread 2: unlocked mutex B\n");
    
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t thread1, thread2;
    
    // Проверяем аргумент командной строки
    if (argc > 1 && atoi(argv[1]) == 1) {
        avoid_deadlock = 1;
        printf("========================================\n");
        printf("DEADLOCK AVOIDANCE MODE\n");
        printf("========================================\n");
        printf("Threads will lock mutexes in SAME order\n\n");
    } else {
        printf("========================================\n");
        printf("DEADLOCK DEMONSTRATION MODE\n");
        printf("========================================\n");
        printf("Threads will lock mutexes in REVERSE order\n");
        printf("This WILL cause a deadlock!\n");
        printf("========================================\n\n");
        printf("To avoid deadlock, run: %s 1\n\n", argv[0]);
    }
    
    // Создаем потоки
    if (pthread_create(&thread1, NULL, thread1_func, NULL) != 0) {
        perror("pthread_create failed for thread1");
        return 1;
    }
    
    if (pthread_create(&thread2, NULL, thread2_func, NULL) != 0) {
        perror("pthread_create failed for thread2");
        return 1;
    }
    
    // Ждем завершения потоков (если они завершатся)
    printf("Waiting for threads to finish...\n");
    printf("(If deadlock occurs, press Ctrl+C to stop)\n\n");
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("\n========================================\n");
    printf("Both threads finished successfully!\n");
    printf("========================================\n");
    
    pthread_mutex_destroy(&mutex_a);
    pthread_mutex_destroy(&mutex_b);
    
    return 0;
}
