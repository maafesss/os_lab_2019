#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // Проверяем аргументы
    if (argc < 4) {
        printf("Usage: %s --seed <seed> --array_size <size> --pnum <num_processes>\n", argv[0]);
        printf("Example: %s --seed 123 --array_size 100 --pnum 4\n", argv[0]);
        return 1;
    }
    
    printf("Parent process: PID = %d\n", getpid());
    printf("Starting sequential_min_max in a separate process...\n\n");
    
    // Создаем дочерний процесс
    pid_t pid = fork();
    
    if (pid < 0) {
        // Ошибка форка
        perror("fork failed");
        return 1;
    } 
    else if (pid == 0) {
        // Дочерний процесс - запускаем sequential_min_max
        printf("Child process: PID = %d\n", getpid());
        printf("Child process: executing sequential_min_max...\n\n");
        
        // Подготавливаем аргументы для exec
        char *args[] = {
            "./sequential_min_max",  // программа
            "--seed", argv[2],       // seed
            "--array_size", argv[4], // array_size
            "--pnum", argv[6],       // pnum
            NULL
        };
        
        // Заменяем процесс на sequential_min_max
        execvp(args[0], args);
        
        // Если exec вернулся - значит ошибка
        perror("exec failed");
        return 1;
    } 
    else {
        // Родительский процесс - ждем завершения дочернего
        int status;
        wait(&status);
        
        printf("\nParent process: Child process finished\n");
        
        if (WIFEXITED(status)) {
            printf("Child process exited with code: %d\n", WEXITSTATUS(status));
        }
        
        printf("Parent process: Done!\n");
    }
    
    return 0;
}
