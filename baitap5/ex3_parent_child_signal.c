// ex3_parent_child_signal.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void child_handler(int signo) {
    (void)signo;
    const char *msg = "Received signal from parent\n";
    write(STDOUT_FILENO, msg, 26);
}

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // child
        if (signal(SIGUSR1, child_handler) == SIG_ERR) {
            perror("signal");
            _exit(1);
        }
        printf("Child pid %d waiting for SIGUSR1...\n", getpid());
        fflush(stdout);
        for (;;) pause();
        _exit(0);
    } else {
        // parent
        printf("Parent pid %d. Will send SIGUSR1 to child (pid %d) 5 times every 2 seconds.\n", getpid(), pid);
        fflush(stdout);
        for (int i = 0; i < 5; ++i) {
            sleep(2);
            if (kill(pid, SIGUSR1) == -1) {
                perror("kill");
                break;
            }
            printf("Parent: sent %d/5\n", i+1);
            fflush(stdout);
        }
        // optional: wait a bit to ensure child handled signals then terminate child gracefully
        sleep(1);
        // send SIGTERM to child to stop it
        kill(pid, SIGTERM);
        wait(NULL);
        printf("Parent done. Exiting.\n");
    }
    return 0;
}
