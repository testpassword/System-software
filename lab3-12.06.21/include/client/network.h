#ifndef SPO_LAB3_NETWORK_H
#define SPO_LAB3_NETWORK_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <memory.h>
#include "../../include/common/error.h"
#include "../../include/common/protocol.h"

int connect_server(char *ip, long port, int *client_socket);
void client_quit(const int *client_socket);
void get_books_net(const int *client_socket, struct book *** books, int *lenght);
void update_book(const int *client_socket, struct book *book);

#endif //SPO_LAB3_NETWORK_H
