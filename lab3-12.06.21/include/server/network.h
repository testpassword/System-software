#ifndef SPO_LAB3_NETWORK_H
#define SPO_LAB3_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int init_connect(int *connect_socket, struct sockaddr_in *server_address, int *reuse, long port, int max_client);

#endif //SPO_LAB3_NETWORK_H
