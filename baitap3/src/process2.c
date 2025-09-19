
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    if (setenv("MY_COMMAND", "ls", 1) != 0) {
        perror("setenv");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
    
        char *cmd = getenv("MY_COMMAND");
        if (!cmd) {
            fprintf(stderr, "Child: MY_COMMAND not set\n");
            _exit(2);
        }
        printf("Child: PID=%d executing command from env: %s\n", getpid(), cmd);
        execlp(cmd, cmd, "-l", (char *)NULL);
        perror("execlp");
        _exit(127);
    } else {
        printf("Parent: PID=%d waiting for child %d\n", getpid(), pid);
        wait(NULL);
        printf("Parent: child done\n");
    }
    return 0;
}
