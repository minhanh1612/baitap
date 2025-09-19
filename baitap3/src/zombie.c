/* zombie.c - tạo zombie để quan sát */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        printf("Child (PID=%d): exiting immediately\n", getpid());
        _exit(0);
    } else {
        printf("Parent (PID=%d): child PID=%d. Now sleep 30s so child becomes zombie.\n", getpid(), pid);
        sleep(30);
        printf("Parent: now calling wait to collect child\n");
        wait(NULL);
        printf("Parent: child reaped, exiting\n");
    }
    return 0;
}
