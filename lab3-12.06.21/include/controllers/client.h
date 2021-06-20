#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>
#include "../views/ui_components.h"
#include "../dtos/BookFrame.h"
#include "../errors.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../models/Command.h"
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

#define CI_SIZE(s) (sizeof(s)/sizeof(Command))
#define KEY_UP_1 259
#define KEY_DEL 127
#define KEY_NL 10

bool key_handle(int ch, size_t *args);
bool cmd_symbol(size_t *args);
bool handle_UP(size_t *args);
bool handle_DOWN(size_t *args);
bool handle_F1(size_t *args);
bool handle_F2(size_t *args);
bool handle_F3(size_t *args);
bool handle_F4(size_t *args);
bool handle_F5(size_t *args);
bool handle_F6(size_t *args);
bool handle_F7(size_t *args);
bool handle_F8(size_t *args);
bool handle_F9(size_t *args);
bool handle_F10(size_t *args);
bool handle_KEY_MOUSE(size_t* args);
int connect_server(char* ip, long port, int* client_socket);
void client_quit(const int* client_socket);