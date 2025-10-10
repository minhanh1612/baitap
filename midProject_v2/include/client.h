#ifndef CLIENT_H
#define CLIENT_H

int connect_to_peer(const char *ip, int port, char *peer_ip, int *peer_port);
int add_connection(int sockfd, const char *ip, int port);
int remove_connection_by_id(int id);
int send_to_connection_id(int id, const char *msg);

void list_connections();

#endif
