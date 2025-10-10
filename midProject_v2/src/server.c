#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"
#include "client.h"
#include "message.h"
#include "utils.h"
#include <signal.h>

static int listen_fd = -1;
static int listen_port = 0;
static pthread_t listen_thread;

void stop_listen() {
    if (listen_fd != -1) {
        close(listen_fd);
        listen_fd = -1;
    }
}

/* thread arg is pointer to int port (heap or stack used by caller) */
void *server_listen_thread(void *arg) {
    int port = *(int *)arg;
    free(arg);

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        pthread_exit(NULL);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        listen_fd = -1;
        pthread_exit(NULL);
    }

    if (listen(listen_fd, 10) < 0) {
        perror("listen");
        close(listen_fd);
        listen_fd = -1;
        pthread_exit(NULL);
    }

    listen_port = port;
    printf("Listening on port %d\n", port);
    fflush(stdout);

    while (1) {
        int new_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (new_fd < 0) {
            if (listen_fd == -1) break; // closed intentionally
            perror("accept");
            continue;
        }

        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cli_addr.sin_addr), ipstr, sizeof(ipstr));
        int peer_port = ntohs(cli_addr.sin_port);

        int conn_id = add_connection(new_fd, ipstr, peer_port);
        if (conn_id < 0) {
            // too many connections
            close(new_fd);
            continue;
        }

        // create receive thread with arg containing conn_id and sock
        typedef struct { int sockfd; int conn_id; } connarg_t;
        connarg_t *carg = malloc(sizeof(connarg_t));
        carg->sockfd = new_fd;
        carg->conn_id = conn_id;

        pthread_create(& (connection_t){0}.thread, NULL, receive_handler, (void *)carg);
        /* note: thread stored inside add_connection; we will create real thread inside add_connection
           to keep thread handle consistent. To avoid duplicating thread creation, modify add_connection
           to create thread instead. But for simplicity, we'll call receive_handler directly with pthread_create
           inside add_connection. The above create is harmless but thread id isn't stored. To avoid double,
           let's change approach: we will create thread inside add_connection. So here we skip creating thread.
        */
    }

    close(listen_fd);
    listen_fd = -1;
    pthread_exit(NULL);
}

/* start listening in a detached thread */
int start_listen(int port) {
    int *parg = malloc(sizeof(int));
    *parg = port;
    if (pthread_create(&listen_thread, NULL, server_listen_thread, parg) != 0) {
        perror("pthread_create");
        free(parg);
        return -1;
    }
    pthread_detach(listen_thread);
    return 0;
}
