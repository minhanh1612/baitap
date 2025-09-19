
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        for (int i=0;i<10;i++) {
            printf("Child: PID=%d, PPID=%d (iteration %d)\n", getpid(), getppid(), i);
            sleep(1);
        }
        printf("Child: done\n");
    } else {
        printf("Parent: PID=%d created child PID=%d. Parent exits immediately.\n", getpid(), pid);
        _exit(0); 
    return 0;
}
