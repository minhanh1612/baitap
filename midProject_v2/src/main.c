#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "utils.h"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <listening_port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    if (port <= 1024 || port > 65535) {
        printf("Choose a valid port > 1024 and <= 65535\n");
        return 1;
    }

    // start server listen thread
    if (start_listen(port) != 0) {
        printf("Failed to start listener\n");
        return 1;
    }
    // allow server to set listening port internally (server prints it)
    // Note: in simplified design, we don't share port back; show user explicitly:
    printf("Server started. Listening port: %d\n", port);

    char line[512];
    char myip[INET_ADDRSTRLEN];
    if (get_local_ip(myip, sizeof(myip)) != 0) strncpy(myip, "127.0.0.1", sizeof(myip));

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        // trim newline
        line[strcspn(line, "\n")] = 0;
        // parse command
        if (strlen(line) == 0) continue;

        char *cmd = strtok(line, " ");
        if (!cmd) continue;

        if (strcmp(cmd, "help") == 0) {
            print_help();
        } else if (strcmp(cmd, "myip") == 0) {
            printf("%s\n", myip);
        } else if (strcmp(cmd, "myport") == 0) {
            printf("%d\n", port);
        } else if (strcmp(cmd, "connect") == 0) {
            char *ip = strtok(NULL, " ");
            char *portstr = strtok(NULL, " ");
            if (!ip || !portstr) {
                printf("Usage: connect <ip> <port>\n");
                continue;
            }
            int p = atoi(portstr);
            if (p <= 0 || p > 65535) {
                printf("Invalid port\n");
                continue;
            }
            // prevent connecting to self
            if (strcmp(ip, myip) == 0 && p == port) {
                printf("Cannot connect to yourself\n");
                continue;
            }
            char peer_ip[INET_ADDRSTRLEN];
            int peer_port;
            int sock = connect_to_peer(ip, p, peer_ip, &peer_port);
            if (sock < 0) {
                printf("Failed to connect to %s:%d\n", ip, p);
            } else {
                int id = add_connection(sock, peer_ip, peer_port);
                if (id < 0) {
                    printf("Too many connections, cannot add\n");
                    close(sock);
                }
            }
        } else if (strcmp(cmd, "list") == 0) {
            list_connections();
        } else if (strcmp(cmd, "terminate") == 0) {
            char *idstr = strtok(NULL, " ");
            if (!idstr) { printf("Usage: terminate <id>\n"); continue; }
            int id = atoi(idstr);
            if (remove_connection_by_id(id) != 0) {
                printf("No such connection id\n");
            }
        } else if (strcmp(cmd, "send") == 0) {
            char *idstr = strtok(NULL, " ");
            char *msg = strtok(NULL, "");
            if (!idstr || !msg) { printf("Usage: send <id> <message>\n"); continue; }
            int id = atoi(idstr);
            if (strlen(msg) > MAX_MSG_LEN) {
                printf("Message too long (max %d chars)\n", MAX_MSG_LEN);
                continue;
            }
            int r = send_to_connection_id(id, msg);
            if (r == -1) printf("Connection id not found\n");
            else if (r == -2) printf("Message too long\n");
            else if (r == -3) printf("Send failed\n");
            else printf("Sent to %d\n", id);
        } else if (strcmp(cmd, "exit") == 0) {
            // terminate all connections and exit
            // call terminate for every possible id
            for (int i = 0; i < 1000; ++i) {
                // try remove, but ignore failures
                remove_connection_by_id(i);
            }
            stop_listen();
            printf("Exiting...\n");
            sleep(1);
            break;
        } else {
            printf("Unknown command. Type help\n");
        }
    }

    return 0;
}
