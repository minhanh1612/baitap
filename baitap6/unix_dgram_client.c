#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCKET_PATH "/tmp/server_dgram_socket"
#define CLIENT_SOCKET_PATH "/tmp/client_dgram_socket"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_un servaddr, cliaddr;
    char buffer[BUFFER_SIZE];
    const char *hello = "Hello from client";

    unlink(CLIENT_SOCKET_PATH);

    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sun_family = AF_UNIX;
    strncpy(cliaddr.sun_path, CLIENT_SOCKET_PATH, sizeof(cliaddr.sun_path) - 1);

    if (bind(sockfd, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SERVER_SOCKET_PATH, sizeof(servaddr.sun_path) - 1);

    sendto(sockfd, hello, strlen(hello), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Hello message sent.\n");

    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    printf("Server sent: %s\n", buffer);

    close(sockfd);
    unlink(CLIENT_SOCKET_PATH);
    return 0;
}