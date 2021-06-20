#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include "../dtos/BookFrame.h"
#include "../errors.h"
#include "../models/Book.h"
#include "../models/Command.h"

#define CI_SIZE(s) (sizeof(s)/sizeof(Command))

struct pthread_args {
    int connect_socket;
    struct sockaddr_in* remote_address;
    socklen_t* socket_len;
    int* client_number;
};

struct ClientConnection {
    int client_socket;
    bool active;
};

int serve(long port);
int init_connect(int* connect_socket, struct sockaddr_in* server_address, int* reuse, long port, int max_client);
bool serverHandler(int ch, size_t *args);