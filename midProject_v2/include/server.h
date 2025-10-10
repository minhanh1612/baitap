#ifndef SERVER_H
#define SERVER_H

#include "utils.h"

void *server_listen_thread(void *arg);
int start_listen(int port);
void stop_listen();

#endif
