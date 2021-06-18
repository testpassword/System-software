#ifndef SPO_LAB3_CLIENT_H
#define SPO_LAB3_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>
#include "../../include/client/view.h"
#include "../models/frames.h"
#include "../errors.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../common/event.h"
#include <stdio.h>

struct pthread_args_client {
    int connect_socket;
    struct Book **books;
    int *lenght;
    bool *connect;
    bool *needUpdate;
};

struct pthread_args_event {
    bool *running;
    int *ch;
    bool *needUpdate;
    size_t *args;
};

int client(char *ip, long port);

#define CI_SIZE(s) (sizeof(s)/sizeof(command))
#define KEY_UP_1 259
#define KEY_DEL 127
#define KEY_NL 10

bool key_handle(int ch, size_t *args);
bool cmd_symbol(size_t *args);
bool cmd_KEY_UP(size_t *args);
bool cmd_KEY_DOWN(size_t *args);
bool cmd_KEY_F1(size_t *args);
bool cmd_KEY_F2(size_t *args);
bool cmd_KEY_F3(size_t *args);
bool cmd_KEY_F4(size_t *args);
bool cmd_KEY_F5(size_t *args);
bool cmd_KEY_F6(size_t *args);
bool cmd_KEY_F7(size_t *args);
bool cmd_KEY_F8(size_t *args);
bool cmd_KEY_F9(size_t *args);
bool cmd_KEY_F10(size_t *args);
bool cmd_KEY_MOUSE(size_t *args);

int connect_server(char *ip, long port, int *client_socket);
void client_quit(const int *client_socket);
void get_books_net(const int *client_socket, struct Book *** books, int *lenght);
void update_book(const int *client_socket, struct Book *book);
#endif
