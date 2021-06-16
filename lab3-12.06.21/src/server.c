#include "../include/server.h"
#include "../include/errors.h"
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

struct ClientConnection client_socket[MAX_CLIENT_NUMBER];
bool running_server = true;
const char* success = "Успех!";
const char* exception = "Неожиданная ошибка сервера, попробуйте попытку позже";

int open_connection(int* connect_socket) {
    println("Соединяюсь...");
    *connect_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect_socket < 0) {
        println("Ошибка соединения");
        return ERR_SERVER_SOCKET_OPEN;
    }
    println(success);
    return SUCCESS;
}

int set_connection_name(const int* connect_socket, int* reuse) {
    println("Настройка подключения...");
    if (setsockopt(*connect_socket, SOL_SOCKET, SO_REUSEADDR, reuse, sizeof(int)) == -1) {
        println("Ошибка конфигурации соединения");
        return ERR_SERVER_SOCKET_SETNAME;
    }
    println(success);
    return SUCCESS;
}

int bind_connection_with_address(const int *connect_socket, struct sockaddr_in *server_address) {
    println("Связываю канал...");
    if (bind(*connect_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        println("Ошибка канала");
        return ERR_SERVER_SOCKET_BIND;
    }
    println(success);
    return SUCCESS;
}

int listen_connection(const int* connect_socket, int max_client) {
    println("Запускаю прослушку...");
    if (listen(*connect_socket, max_client) == -1) {
        println("Ошибка прослушки: обратитесь к своему агенту ФСБ для дальнейший указаний");
        return ERR_SERVER_SOCKET_LISTEN;
    }
    println(success);
    return SUCCESS;
}

int init_connect(int *connect_socket, struct sockaddr_in *server_address, int *reuse, long port, int max_client) {
    int errno = SUCCESS;
    errno = open_connection(connect_socket);
    if (errno != SUCCESS) return errno;
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    server_address->sin_addr.s_addr = htonl(INADDR_ANY);
    errno = set_connection_name(connect_socket, reuse);
    if (errno != SUCCESS) return errno;
    errno = bind_connection_with_address(connect_socket, server_address);
    if (errno != SUCCESS) return errno;
    errno = listen_connection(connect_socket, max_client);
    if (errno != SUCCESS) return errno;
    return SUCCESS;
}

void *accept_pthread(void *args) {
    struct pthread_args *arg = args;
    int connect_socket = arg->connect_socket;
    struct sockaddr *remote_address = (struct sockaddr *) arg->remote_address;
    socklen_t *socket_len = arg->socket_len;
    int *client_number = arg->client_number;
    struct Frame config_frame;
    while (running_server) {
        client_socket[*client_number].client_socket = accept(connect_socket, remote_address, socket_len);
        if (client_socket[*client_number].client_socket < 0) {
            println(exception);
            exit(ERR_SERVER_ACCEPT);
        }
        if (*client_number < MAX_CLIENT_NUMBER) {
            println("Клиент %d/%d подключился", *client_number + 1, MAX_CLIENT_NUMBER);
            config_frame.function = SERVER_FULL;
            config_frame.function_parameter = 0;
            pack_frame(client_socket[*client_number].client_socket, &config_frame);
            client_socket[*client_number].active = true;
            (*client_number)++;
        } else if (*client_number == MAX_CLIENT_NUMBER) println("Сервер загружен по максимуму, повторите попытку позже");
        else {
            config_frame.function = SERVER_FULL;
            println(exception);
            pack_frame(client_socket[*client_number].client_socket, &config_frame);
            close(client_socket[*client_number].client_socket);
            client_socket[*client_number].active = false;
        }
    }
    return NULL;
}

void terminate_serve_process() {
    struct Frame configFrame = {.function = SERVER_QUIT, .function_parameter = 0};
    for (size_t i = 0; i < MAX_CLIENT_NUMBER; i++)
        if (client_socket[i].client_socket > -1 && client_socket[i].active) {
            pack_frame(client_socket[i].client_socket, &configFrame);
            close(client_socket[i].client_socket);
        }
    running_server = false;
}

void interrupt_close_server() {
    terminate_serve_process();
    exit(SIGINT);
}

void handler(const int *client_number, struct Book **books, const int lenght_book) {
    struct Frame command;
    fd_set read_fds;
    struct timeval tv = {.tv_sec=0, .tv_usec=1000};
    size_t *argsEvent = calloc(5, sizeof(size_t));
    argsEvent[0] = (size_t) 0;
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
            if (ret < 0) println(exception);
            else if (ret == 0) continue;
            else {
                unpack_frame(client_socket[i].client_socket, &command);
                argsEvent[0] = i;
                serverHandler(command.function, argsEvent);
            }
        }
    }
    free(argsEvent);
}

int serve(long port) {
    signal(SIGINT, interrupt_close_server);
    int connect_socket = 0;
    struct sockaddr_in server_address;
    int reuse = 1;
    int errno = init_connect(&connect_socket, &server_address, &reuse, port, MAX_CLIENT_NUMBER);
    if (errno != 0) return errno;
    struct sockaddr_in remote_address;
    socklen_t socket_len = sizeof(remote_address);
    int client_number = 0;
    struct Book **books = read_book();
    int lenght_book = count_books(books);
    pthread_t threadAccept;
    struct pthread_args args = {connect_socket, &remote_address, &socket_len, &client_number};
    pthread_create(&threadAccept, NULL, accept_pthread, &args);
    handler(&client_number, books, lenght_book);
    terminate_serve_process();
    return SUCCESS;
}

bool com_default(size_t *args) {
    size_t i = args[0];
    struct ClientConnection *client_sockets = (struct ClientConnection *) args[1];
    println("Нечто поступило на сервер (возможно смертельный файл). Смотреть не рекомендуется");
    close(client_sockets[i].client_socket);
    client_sockets[i].active = false;
    return true;
}

bool com_get_all_book(size_t *args) {
    size_t i = args[0];
    struct ClientConnection *client_sockets = (struct ClientConnection *) args[1];
    size_t lenght_book = args[2];
    struct Book **books = (struct Book **) args[3];
    printf("Client #%zu: Get All\n", i);
    struct BookFrame bookFrame;
    for (int lbook = 0; lbook < lenght_book; lbook++) {
        if (books[lbook]) {
            bookFrame.book = *(books[lbook]);
            bookFrame.function = SEND_BOOK;
        } else {
            bookFrame.book = (struct Book) {};
            bookFrame.function = SEND_BOOK_EOF;
        }
        pack_book(client_sockets[i].client_socket, &bookFrame);
    }
    return false;
}

bool com_client_quit(size_t *args) {
    size_t i = args[0];
    struct ClientConnection *client_sockets = (struct ClientConnection *) args[1];
    printf("Client #%zu: Quit\n", i);
    close(client_sockets[i].client_socket);
    client_sockets[i].active = false;
    return true;
}

void packAll(const int *client_number, struct ClientConnection *client_sockets, struct Frame *configFrame) {
    for (int i = 0; i < *client_number; i++)
        if (client_sockets[i].client_socket > 0 && client_sockets[i].active)
            pack_frame(client_sockets[i].client_socket, configFrame);
}

bool com_client_update_book(size_t *args) {
    size_t i = args[0];
    struct ClientConnection *client_sockets = (struct ClientConnection *) args[1];
    size_t lenght_book = args[2];
    struct Book **books = (struct Book **) args[3];
    int *client_number = (int *) args[4];
    printf("Client #%zu: update Book\n", i);
    struct BookFrame bookFrameUpdate;
    unpack_book(client_sockets[i].client_socket, &bookFrameUpdate);
    for (int lbook = 0; lbook < lenght_book; lbook++)
        if (books[lbook]->bookID == bookFrameUpdate.book.bookID) {
            memcpy(books[lbook], &(bookFrameUpdate.book), sizeof(struct Book));
            break;
        }
    struct Frame configFrame;
    configFrame.function = SEND_UPDATE_INFO;
    packAll(client_number, client_sockets, &configFrame);
    return true;
}

command knownCommandServer[] = {
        {DEFAULT_EVENT,      &com_default},
        {GET_ALL_BOOK,       &com_get_all_book},
        {CLIENT_QUIT,        &com_client_quit},
        {CLIENT_UPDATE_BOOK, &com_client_update_book},
};

bool serverHandler(int ch, size_t *args) { return event(ch, args, knownCommandServer, CI_SIZE(knownCommandServer)); }