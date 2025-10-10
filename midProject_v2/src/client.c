#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "client.h"
#include "utils.h"
#include "message.h"

static connection_t connections[MAX_CONN];
static pthread_mutex_t conn_lock = PTHREAD_MUTEX_INITIALIZER;
static int next_id = 1;
static int current_listen_port = 0;

/* internal: find free slot, add connection, create receive thread */
int add_connection(int sockfd, const char *ip, int port) {
    pthread_mutex_lock(&conn_lock);
    int idx = -1;
    for (int i = 0; i < MAX_CONN; ++i) {
        if (!connections[i].active) { idx = i; break; }
    }
    if (idx == -1) {
        pthread_mutex_unlock(&conn_lock);
        return -1;
    }
    connections[idx].active = 1;
    connections[idx].sockfd = sockfd;
    connections[idx].id = next_id++;
    strncpy(connections[idx].ip, ip, INET_ADDRSTRLEN-1);
    connections[idx].ip[INET_ADDRSTRLEN-1] = '\0';
    connections[idx].port = port;
    int conn_id = connections[idx].id;
    // create recv thread: pass conn_id and sock
    typedef struct { int sockfd; int conn_id; } connarg_t;
    connarg_t *carg = malloc(sizeof(connarg_t));
    carg->sockfd = sockfd;
    carg->conn_id = conn_id;

    if (pthread_create(&connections[idx].thread, NULL, receive_handler, (void *)carg) != 0) {
        close(sockfd);
        connections[idx].active = 0;
        free(carg);
        pthread_mutex_unlock(&conn_lock);
        return -1;
    }
    pthread_detach(connections[idx].thread);
    pthread_mutex_unlock(&conn_lock);
    printf("Connection added: id=%d, %s:%d\n", conn_id, ip, port);
    return conn_id;
}

int remove_connection_by_id(int id) {
    pthread_mutex_lock(&conn_lock);
    for (int i = 0; i < MAX_CONN; ++i) {
        if (connections[i].active && connections[i].id == id) {
            close(connections[i].sockfd);
            connections[i].active = 0;
            // thread will exit on recv error/closed socket
            pthread_mutex_unlock(&conn_lock);
            printf("Terminated connection %d\n", id);
            return 0;
        }
    }
    pthread_mutex_unlock(&conn_lock);
    return -1;
}

int send_to_connection_id(int id, const char *msg) {
    pthread_mutex_lock(&conn_lock);
    int sock = -1;
    for (int i = 0; i < MAX_CONN; ++i) {
        if (connections[i].active && connections[i].id == id) {
            sock = connections[i].sockfd;
            break;
        }
    }
    pthread_mutex_unlock(&conn_lock);
    if (sock == -1) return -1;
    int len = strlen(msg);
    if (len > MAX_MSG_LEN) return -2;
    // send length-limited message
    if (send(sock, msg, len, 0) < 0) {
        return -3;
    }
    return 0;
}

void list_connections() {
    pthread_mutex_lock(&conn_lock);
    printf("ID\tIP\t\tPORT\n");
    for (int i = 0; i < MAX_CONN; ++i) {
        if (connections[i].active) {
            printf("%d\t%s\t%d\n", connections[i].id, connections[i].ip, connections[i].port);
        }
    }
    pthread_mutex_unlock(&conn_lock);
}

/* Called by server to set listening port so main can print myport */
void set_listen_port(int port) {
    current_listen_port = port;
}

int get_listen_port() {
    return current_listen_port;
}
/* connect_to_peer implementation */
int connect_to_peer(const char *ip, int port, char *peer_ip, int *peer_port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &serv.sin_addr) <= 0) {
        close(sockfd);
        return -1;
    }
    if (connect(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        close(sockfd);
        return -1;
    }
    // fill peer info
    strncpy(peer_ip, ip, INET_ADDRSTRLEN-1);
    peer_ip[INET_ADDRSTRLEN-1] = '\0';
    *peer_port = port;
    return sockfd;
}
