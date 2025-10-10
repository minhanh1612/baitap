#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "message.h"
#include "client.h"
#include "utils.h"
#include <arpa/inet.h>
#include <sys/socket.h>

/* argument passed is pointer to struct {int sockfd; int conn_id;} */
void *receive_handler(void *arg) {
    struct {
        int sockfd;
        int conn_id;
    } *carg = arg;

    int sock = carg->sockfd;
    int conn_id = carg->conn_id;
    free(carg);

    char buf[1024];
    while (1) {
        ssize_t r = recv(sock, buf, sizeof(buf)-1, 0);
        if (r <= 0) {
            // peer closed or error
            // remove from connection list by conn_id
            remove_connection_by_id(conn_id);
            close(sock);
            break;
        }
        buf[r] = '\0';
        // print message with sender info
        printf("\n[Message from conn %d] %s\n> ", conn_id, buf);
        fflush(stdout);
    }
    return NULL;
}
