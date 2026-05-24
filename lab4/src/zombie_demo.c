#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0) {
        // Дочерний процесс
        printf("Child process: PID = %d\n", getpid());
        printf("Child process: Parent PID = %d\n", getppid());
        printf("Child process: Exiting...\n");
        exit(0);
    }
    else {
        // Родительский процесс
        printf("Parent process: PID = %d\n", getpid());
        printf("Parent process: Child PID = %d\n", pid);
        printf("Parent process: Sleeping for 30 seconds...\n");
        printf("During this time, child becomes ZOMBIE!\n");
        printf("Run in another terminal: ps aux | grep Z\n\n");
        
        sleep(30);
        
        // Теперь родитель вызывает wait() - зомби исчезает
        int status;
        wait(&status);
        printf("\nParent process: Called wait() - zombie is gone!\n");
        printf("Parent process: Child exited with status %d\n", WEXITSTATUS(status));
    }
    
    return 0;
}
