#ifndef UTILS_H
#define UTILS_H
#include <netinet/in.h>
#define MAX_MSG_LEN 100
#define MAX_CONN 100

typedef struct {
    int id;
    int sockfd;
    char ip[INET_ADDRSTRLEN];
    int port;
    pthread_t thread;
    int active;
} connection_t;

void print_help();
int get_local_ip(char *buf, size_t buflen);
void perror_exit(const char *msg);

#endif
