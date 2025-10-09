#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t seconds = 0;

void alarm_handler(int signo) {
    (void)signo;
    seconds++;
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "Timer: %d seconds\n", seconds);
    if (n > 0) write(STDOUT_FILENO, buf, (size_t)n);
    if (seconds >= 10) {
        const char *end = "Reached 10 seconds. Exiting.\n";
        write(STDOUT_FILENO, end, 27);
        _exit(0);
    }
    alarm(1); // re-arm for next second
}

int main(void) {
    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }
    printf("Starting timer (1s tick). Will stop after 10 seconds.\n");
    fflush(stdout);
    alarm(1); // start
    while (1) pause();
    return 0;
}
