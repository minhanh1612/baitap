
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("Child: PID=%d, PPID=%d\n", getpid(), getppid());
        printf("Child: will exit with code 10\n");
        _exit(10);
    } else {
        printf("Parent: PID=%d created child PID=%d\n", getpid(), pid);
        int status;
        pid_t w = wait(&status);
        if (w == -1) {
            perror("wait");
            return 1;
        }
        if (WIFEXITED(status)) {
            printf("Parent: child %d exited with status %d\n", w, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Parent: child %d killed by signal %d\n", w, WTERMSIG(status));
        } else {
            printf("Parent: child %d ended with other status\n", w);
        }
    }
    return 0;
}
