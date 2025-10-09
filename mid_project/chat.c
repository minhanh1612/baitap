// chat.c
// Simple P2P CLI chat (TCP) -- follows the project spec in your PDF.
// Compile: gcc -Wall -Wextra -pthread -o chat chat.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define MAX_CONN 100
#define MAX_MSG 100
#define BUF_SIZE 1024

typedef struct {
    int used;               // 0 = free, 1 = occupied
    int id;                 // unique connection ID
    int sockfd;             // socket file descriptor
    struct sockaddr_in addr; // peer address (stored at creation)
    pthread_t thread;
} connection_t;

static connection_t conns[MAX_CONN];
static pthread_mutex_t conns_mutex = PTHREAD_MUTEX_INITIALIZER;
static int next_id = 1;

static int server_sock = -1;
static int listening_port = 0;
static volatile int server_running = 1;

/* Utility: get local IP (best-effort) */
const char* get_my_ip(char *buf, size_t buflen) {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        strncpy(buf, "127.0.0.1", buflen);
        return buf;
    }
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("8.8.8.8");
    serv.sin_port = htons(53);
    /* doesn't have to succeed connecting to remote - just used to find local interface */
    connect(s, (struct sockaddr*)&serv, sizeof(serv));
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    if (getsockname(s, (struct sockaddr*)&name, &namelen) == 0) {
        inet_ntop(AF_INET, &name.sin_addr, buf, buflen);
    } else {
        strncpy(buf, "127.0.0.1", buflen);
    }
    close(s);
    return buf;
}

/* Forward */
void *recv_handler(void *arg);

/* Add new connection into table and start a recv thread */
int add_connection(int sockfd, struct sockaddr_in addr) {
    pthread_mutex_lock(&conns_mutex);
    int i;
    for (i = 0; i < MAX_CONN; ++i) {
        if (!conns[i].used) {
            conns[i].used = 1;
            conns[i].sockfd = sockfd;
            conns[i].addr = addr;
            conns[i].id = next_id++;
            int *idx = malloc(sizeof(int));
            if (!idx) {
                perror("malloc");
                pthread_mutex_unlock(&conns_mutex);
                return -1;
            }
            *idx = i;
            if (pthread_create(&conns[i].thread, NULL, recv_handler, idx) != 0) {
                perror("pthread_create");
                free(idx);
                conns[i].used = 0;
                pthread_mutex_unlock(&conns_mutex);
                return -1;
            }
            pthread_detach(conns[i].thread);
            pthread_mutex_unlock(&conns_mutex);
            return conns[i].id;
        }
    }
    pthread_mutex_unlock(&conns_mutex);
    return -1;
}

/* Find slot index by connection id; returns -1 if not found */
int find_slot_by_id(int id) {
    int i;
    for (i = 0; i < MAX_CONN; ++i) {
        if (conns[i].used && conns[i].id == id) return i;
    }
    return -1;
}

/* Recv thread: listen for messages on a single connection */
void *recv_handler(void *arg) {
    int idx = *((int*)arg);
    free(arg);

    char buf[BUF_SIZE];
    while (1) {
        pthread_mutex_lock(&conns_mutex);
        if (!conns[idx].used) {
            pthread_mutex_unlock(&conns_mutex);
            break;
        }
        int fd = conns[idx].sockfd;
        struct sockaddr_in addr = conns[idx].addr;
        pthread_mutex_unlock(&conns_mutex);

        ssize_t n = recv(fd, buf, BUF_SIZE - 1, 0);
        if (n > 0) {
            buf[n] = '\0';
            printf("\n****************************************\n");
            printf("Message received from: %s\n", inet_ntoa(addr.sin_addr));
            printf("Sender's port: %d\n", ntohs(addr.sin_port));
            printf("Message: %s\n", buf);
            printf("****************************************\n");
            fflush(stdout);
        } else {
            pthread_mutex_lock(&conns_mutex);
            if (conns[idx].used) {
                printf("\nThe peer at port %d has disconnected.\n", ntohs(conns[idx].addr.sin_port));
                close(conns[idx].sockfd);
                conns[idx].used = 0;
            }
            pthread_mutex_unlock(&conns_mutex);
            break;
        }
    }
    return NULL;
}

/* Accept thread: accept incoming connections and register them */
void *accept_thread(void *arg) {
    (void)arg;
    while (server_running) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int clientfd = accept(server_sock, (struct sockaddr*)&client, &len);
        if (clientfd < 0) {
            if (!server_running) break;
            perror("accept");
            continue;
        }
        int id = add_connection(clientfd, client);
        printf("Accepted a new connection from address: %s, setup at port: %d\n",
               inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        if (id >= 0) {
            printf("ID: %d\n", id);
        } else {
            printf("Failed to add connection (table full)\n");
            close(clientfd);
        }
        fflush(stdout);
    }
    return NULL;
}

/* Print help */
void print_help() {
    puts("Use the commands below:");
    puts("1. help                       : display user interface options");
    puts("2. myip                       : display IP address of this app");
    puts("3. myport                     : display listening port of this app");
    puts("4. connect <destination> <port>: connect to the app of another computer");
    puts("5. list                       : list all the connections of this app");
    puts("6. terminate <connection id>  : terminate a connection");
    puts("7. send <connection id> <msg> : send a message to a connection (<=100 chars)");
    puts("8. exit                       : close all connections & terminate this app");
}

/* Print list of current connections */
void cmd_list() {
    pthread_mutex_lock(&conns_mutex);
    printf("ID\tIP Address\t\tPort No.\n");
    for (int i = 0; i < MAX_CONN; ++i) {
        if (conns[i].used) {
            printf("%d\t%s\t\t%d\n", conns[i].id,
                   inet_ntoa(conns[i].addr.sin_addr),
                   ntohs(conns[i].addr.sin_port));
        }
    }
    pthread_mutex_unlock(&conns_mutex);
}

/* Terminate connection by ID */
void cmd_terminate(int id) {
    pthread_mutex_lock(&conns_mutex);
    int slot = find_slot_by_id(id);
    if (slot == -1) {
        printf("Error: connection id %d does not exist.\n", id);
        pthread_mutex_unlock(&conns_mutex);
        return;
    }
    close(conns[slot].sockfd);
    conns[slot].used = 0;
    printf("terminate peer with ID %d successfully.\n", id);
    pthread_mutex_unlock(&conns_mutex);
}

/* Send message to a connection id */
void cmd_send(int id, const char *message) {
    if (!message) {
        printf("Error: missing message.\n");
        return;
    }
    if ((int)strlen(message) > MAX_MSG) {
        printf("Error: message too long (max %d chars).\n", MAX_MSG);
        return;
    }
    pthread_mutex_lock(&conns_mutex);
    int slot = find_slot_by_id(id);
    if (slot == -1) {
        printf("Error: connection id %d does not exist.\n", id);
        pthread_mutex_unlock(&conns_mutex);
        return;
    }
    int fd = conns[slot].sockfd;
    ssize_t sent = send(fd, message, strlen(message), 0);
    if (sent < 0) {
        perror("send");
    } else {
        printf("Sent message successfully.\n");
    }
    pthread_mutex_unlock(&conns_mutex);
}

/* Connect to remote ip:port */
void cmd_connect(const char *ip, int port) {
    if (!ip || port <= 0 || port > 65535) {
        printf("Error: invalid ip/port.\n");
        return;
    }
    char myip[64];
    get_my_ip(myip, sizeof(myip));
    if (strcmp(ip, myip) == 0 && port == listening_port) {
        printf("Error: cannot connect to yourself.\n");
        return;
    }

    // Check duplicate: we won't allow connecting twice to same ip:port (as seen from our side)
    pthread_mutex_lock(&conns_mutex);
    for (int i = 0; i < MAX_CONN; ++i) {
        if (conns[i].used) {
            if (strcmp(inet_ntoa(conns[i].addr.sin_addr), ip) == 0 &&
                ntohs(conns[i].addr.sin_port) == port) {
                printf("Error: connection to %s:%d already exists.\n", ip, port);
                pthread_mutex_unlock(&conns_mutex);
                return;
            }
        }
    }
    pthread_mutex_unlock(&conns_mutex);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &dest.sin_addr) <= 0) {
        printf("Error: invalid IP address format.\n");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    int id = add_connection(sock, dest);
    if (id < 0) {
        printf("Failed to add connection (table full).\n");
        close(sock);
        return;
    }
    printf("Connected successfully. Ready for data transmission\n");
}

/* Cleanup on exit */
void cleanup_and_exit() {
    server_running = 0;
    if (server_sock >= 0) {
        close(server_sock);
    }
    pthread_mutex_lock(&conns_mutex);
    for (int i = 0; i < MAX_CONN; ++i) {
        if (conns[i].used) {
            close(conns[i].sockfd);
            conns[i].used = 0;
        }
    }
    pthread_mutex_unlock(&conns_mutex);
    printf("All connections closed. Exiting.\n");
    exit(0);
}

/* Entry point */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <listening_port>\n", argv[0]);
        return 1;
    }
    listening_port = atoi(argv[1]);
    if (listening_port <= 0 || listening_port > 65535) {
        fprintf(stderr, "Invalid port.\n");
        return 1;
    }

    /* init connection table */
    for (int i = 0; i < MAX_CONN; ++i) conns[i].used = 0;

    /* create server socket */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        return 1;
    }
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(listening_port);
    if (bind(server_sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
        perror("bind");
        close(server_sock);
        return 1;
    }
    if (listen(server_sock, 10) < 0) {
        perror("listen");
        close(server_sock);
        return 1;
    }

    printf("Application is listening on port: %d\n", listening_port);
    print_help();

    pthread_t at;
    if (pthread_create(&at, NULL, accept_thread, NULL) != 0) {
        perror("pthread_create");
        close(server_sock);
        return 1;
    }
    pthread_detach(at);

    /* command loop */
    char line[1024];
    while (1) {
        printf("\nEnter your command: ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) {
            puts("EOF or read error - exiting");
            cleanup_and_exit();
        }
        /* trim newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        /* parse */
        char *cmd = strtok(line, " ");
        if (!cmd) continue;

        if (strcmp(cmd, "help") == 0) {
            print_help();
        } else if (strcmp(cmd, "myip") == 0) {
            char ip[64];
            get_my_ip(ip, sizeof(ip));
            printf("%s\n", ip);
        } else if (strcmp(cmd, "myport") == 0) {
            printf("%d\n", listening_port);
        } else if (strcmp(cmd, "connect") == 0) {
            char *ip = strtok(NULL, " ");
            char *ps = strtok(NULL, " ");
            if (!ip || !ps) {
                printf("Usage: connect <destination_ip> <port>\n");
                continue;
            }
            int p = atoi(ps);
            cmd_connect(ip, p);
        } else if (strcmp(cmd, "list") == 0) {
            cmd_list();
        } else if (strcmp(cmd, "terminate") == 0) {
            char *ids = strtok(NULL, " ");
            if (!ids) {
                printf("Usage: terminate <connection_id>\n");
                continue;
            }
            int id = atoi(ids);
            cmd_terminate(id);
        } else if (strcmp(cmd, "send") == 0) {
            char *ids = strtok(NULL, " ");
            if (!ids) {
                printf("Usage: send <connection_id> <message>\n");
                continue;
            }
            int id = atoi(ids);
            /* remaining string is the message (allow spaces) */
            char *msg = strtok(NULL, "");
            if (!msg) {
                printf("Usage: send <connection_id> <message>\n");
                continue;
            }
            cmd_send(id, msg);
        } else if (strcmp(cmd, "exit") == 0) {
            cleanup_and_exit();
        } else {
            printf("Unknown command: %s\n", cmd);
            print_help();
        }
    }

    return 0;
}
