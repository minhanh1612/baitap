// ex4_select_signal_input.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>

volatile sig_atomic_t got_sigint = 0;
volatile sig_atomic_t got_sigterm = 0;

void sigint_handler(int signo) {
    (void)signo;
    const char *msg = "SIGINT received.\n";
    write(STDOUT_FILENO, msg, 16);
    got_sigint = 1;
}

void sigterm_handler(int signo) {
    (void)signo;
    const char *msg = "SIGTERM received. Exiting.\n";
    write(STDOUT_FILENO, msg, 23);
    got_sigterm = 1;
}

int main(void) {
    struct sigaction sa1, sa2;
    sa1.sa_handler = sigint_handler;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;

    sa2.sa_handler = sigterm_handler;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;

    if (sigaction(SIGINT, &sa1, NULL) == -1) {
        perror("sigaction SIGINT");
        return 1;
    }
    if (sigaction(SIGTERM, &sa2, NULL) == -1) {
        perror("sigaction SIGTERM");
        return 1;
    }

    printf("Enter text and press Enter to echo. Send SIGINT to get message, SIGTERM to quit.\n");
    fflush(stdout);

    while (!got_sigterm) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        int nfds = STDIN_FILENO + 1;

        int rv = select(nfds, &readfds, NULL, NULL, NULL);
        if (rv == -1) {
            if (errno == EINTR) {
                if (got_sigterm) break;
                continue;
            } else {
                perror("select");
                break;
            }
        } else if (rv > 0) {
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char buf[1024];
                ssize_t n = read(STDIN_FILENO, buf, sizeof(buf)-1);
                if (n > 0) {
                    buf[n] = '\0';
                    size_t len = strlen(buf);
                    if (len>0 && buf[len-1]=='\n') buf[len-1]='\0';
                    printf("You typed: %s\n", buf);
                    fflush(stdout);
                } else if (n == 0) {
                    // EOF
                    printf("EOF on stdin. Exiting.\n");
                    break;
                }
            }
        }
    }

    printf("Program exiting.\n");
    return 0;
}
