#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;

void handler(int signo) {
    (void)signo;
    sigint_count++;
    const char *msg = "SIGINT received\n";
    write(STDOUT_FILENO, msg, 16); 
    if (sigint_count >= 3) {
        const char *end = "Received SIGINT 3 times. Exiting.\n";
        write(STDOUT_FILENO, end, 33);
        _exit(0);
    }
}

int main(void) {
    if (signal(SIGINT, handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    printf("PID %d. Press Ctrl+C up to 3 times to exit.\n", getpid());
    fflush(stdout);
it
    while (1) {
        pause(); 
    }
    return 0;
}
