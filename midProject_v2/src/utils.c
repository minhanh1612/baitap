#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "utils.h"
#include <sys/socket.h>
#include <ifaddrs.h>

void print_help() {
    printf("Available commands:\n");
    printf("help                       : Show this help\n");
    printf("myip                       : Show this machine IP\n");
    printf("myport                     : Show listening port\n");
    printf("connect <ip> <port>        : Connect to a peer\n");
    printf("list                       : List active connections\n");
    printf("terminate <connection_id>  : Terminate connection by id\n");
    printf("send <connection_id> <msg> : Send message (<= %d chars)\n", MAX_MSG_LEN);
    printf("exit                       : Close all and exit\n");
}

/* get first non-loopback IPv4 address */
int get_local_ip(char *buf, size_t buflen) {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) return -1;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            char addr[INET_ADDRSTRLEN];
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), addr, sizeof(addr));
            if (strcmp(addr, "127.0.0.1") != 0) {
                strncpy(buf, addr, buflen-1);
                buf[buflen-1] = '\0';
                freeifaddrs(ifaddr);
                return 0;
            }
        }
    }
    /* fallback to loopback if none */
    strncpy(buf, "127.0.0.1", buflen-1);
    buf[buflen-1] = '\0';
    freeifaddrs(ifaddr);
    return 0;
}

void perror_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
