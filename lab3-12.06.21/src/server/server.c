#include "../../include/server/server.h"
#include "../../include/server/serverEvent.h"

struct _client_socket client_socket[MAX_CLIENT_NUMBER];
bool running_server = true;

void *accept_pthread(void *args) {
    struct pthread_args *arg = args;
    int connect_socket = arg->connect_socket;
    struct sockaddr *remote_address = (struct sockaddr *) arg->remote_address;
    socklen_t *socket_len = arg->socket_len;
    int *client_number = arg->client_number;
    struct _config_frame config_frame;
    while (running_server) {
        client_socket[*client_number].client_socket = accept(connect_socket, remote_address, socket_len);
        if (client_socket[*client_number].client_socket < 0) {
            perror("Error: 'accept()'");
            exit(ERR_SERVER_ACCEPT);
        }
        if (*client_number < MAX_CLIENT_NUMBER) {
            printf("Client №%d has been connected\n", *client_number);
            config_frame.function = SERVER_FULL;
            config_frame.function_parameter = 0;
            pack(client_socket[*client_number].client_socket, &config_frame);
            client_socket[*client_number].active = true;
            (*client_number)++;
            if (*client_number == MAX_CLIENT_NUMBER) printf("Server is full\n");
        } else {
            printf("New client can`t connect:");
            config_frame.function = SERVER_FULL;
            if (*client_number == MAX_CLIENT_NUMBER) {
                printf(" 'Server is full'");
                config_frame.function_parameter = 1;
            } else {
                printf(" 'Unknown Error'");
                config_frame.function_parameter = 0;
            }
            printf("\n");
            pack(client_socket[*client_number].client_socket, &config_frame);
            close(client_socket[*client_number].client_socket);
            client_socket[*client_number].active = false;
        }
    }
    return NULL;
}

void server_quit() {
    struct _config_frame configFrame = {.function = SERVER_QUIT, .function_parameter = 0};
    for (size_t i = 0; i < MAX_CLIENT_NUMBER; i++)
        if (client_socket[i].client_socket > -1 && client_socket[i].active) {
            pack(client_socket[i].client_socket, &configFrame);
            close(client_socket[i].client_socket);
        }
    running_server = false;
}

void interrupt_close_server() {
    server_quit();
    exit(SIGINT);
}

void handler(const int *client_number, struct book **books, const int lenght_book) {
    struct _config_frame command;
    fd_set read_fds;
    struct timeval tv = {.tv_sec=0, .tv_usec=1000};
    size_t *argsEvent = calloc(5, sizeof(size_t));
    argsEvent[0] = (size_t) 0; // Client Number
    argsEvent[1] = (size_t) client_socket;
    argsEvent[2] = lenght_book;
    argsEvent[3] = (size_t) books;
    argsEvent[4] = (size_t) client_number;
    while (running_server) {
        FD_ZERO(&read_fds);
        for (int i = 0; i < *client_number; i++) {
            if (client_socket[i].client_socket <= 0 || !client_socket[i].active) { continue; }
            FD_SET(client_socket[i].client_socket, &read_fds);
            int ret = select(client_socket[i].client_socket + 1, &read_fds, NULL, NULL, &tv);
            if (ret < 0) perror("Some Error ");
            else if (ret == 0) continue;
            else {
                unpack(client_socket[i].client_socket, &command);
                argsEvent[0] = i;
                serverHandler(command.function, argsEvent);
            }
        }
    }
    free(argsEvent);
}

int server(long port) {
    signal(SIGINT, interrupt_close_server);
    int connect_socket = 0;
    struct sockaddr_in server_address;
    int reuse = 1;
    int errno = init_connect(&connect_socket, &server_address, &reuse, port, MAX_CLIENT_NUMBER);
    if (errno != 0) { return errno; }
    struct sockaddr_in remote_address;
    socklen_t socket_len = sizeof(remote_address);
    int client_number = 0;
    struct book **books = read_book();
    int lenght_book = get_lenght_book(books);
    pthread_t threadAccept;
    struct pthread_args args = {connect_socket, &remote_address, &socket_len, &client_number};
    pthread_create(&threadAccept, NULL, accept_pthread, &args);
    handler(&client_number, books, lenght_book);
    server_quit();
    return SUCCESS;
}