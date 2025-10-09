#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCKET_PATH "/tmp/server_dgram_socket"
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_un servaddr, cliaddr;
    char buffer[BUFFER_SIZE];
    const char *hello = "Hello from server";

    unlink(SERVER_SOCKET_PATH);

    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SERVER_SOCKET_PATH, sizeof(servaddr.sun_path) - 1);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on %s\n", SERVER_SOCKET_PATH);

    unsigned int len = sizeof(cliaddr);
    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
    printf("Client sent: %s\n", buffer);

    sendto(sockfd, hello, strlen(hello), 0, (const struct sockaddr *)&cliaddr, len);
    printf("Hello message sent.\n");

    close(sockfd);
    unlink(SERVER_SOCKET_PATH);
    return 0;
}