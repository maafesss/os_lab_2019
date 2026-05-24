#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("Child: PID=%d, exiting...\n", getpid());
        exit(0);
    } else {
        printf("Parent: PID=%d, Child PID=%d\n", getpid(), pid);
        printf("Parent in infinite loop - zombie forever!\n");
        while (1) sleep(10);
    }
    return 0;
}
