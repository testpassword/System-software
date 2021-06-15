#include "../../include/client/network.h"

int open_socket(int *client_socket) {
    printf("Open socket.....................................");
    *client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*client_socket < 0) {
        printf("faild\n");
        perror("Error: 'socket()'");
        return ERR_CLIENT_OPEN_SOCKET;
    }
    printf("done\n");
    return SUCCESS;
}

int connect_to_server(const int *client_socket, struct sockaddr_in *server_address) {
    printf("Connection to server............................");
    if (connect(*client_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        printf("faild\n");
        perror("Error: 'connect()'");
        return ERR_CLIENT_CONNECT;
    }
    printf("done\n");
    return SUCCESS;
}

int check_connect(const int *client_socket) {
    struct _config_frame config_frame;
    unpack(*client_socket, &config_frame);
    if (!((config_frame.function == SERVER_FULL) && (config_frame.function_parameter == 0))) {
        close(*client_socket);
        return ERR_CLIENT_CONNECT_SERVER;
    }
    return SUCCESS;
}

int connect_server(char *ip, long port, int *client_socket) {
    int err = SUCCESS;
    err = open_socket(client_socket);
    if (err != SUCCESS) return err;
    struct sockaddr_in server_address = {.sin_family = AF_INET, .sin_port = htons(port)};
    struct in_addr serv_address;
    if (inet_aton(ip, &serv_address) != 0) server_address.sin_addr = serv_address;
    else server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    err = connect_to_server(client_socket, &server_address);
    if (err != SUCCESS) return err;
    err = check_connect(client_socket);
    if (err != SUCCESS) return err;

/**
 * char client_name[MAX_USER_NAME_LENGTH] = {""};
    printf("Input your name or press 'Enter' to 'Player': ");
    if (fgets(client_name, MAX_USER_NAME_LENGTH, stdin)) {
        if(client_name[0] == '\n') {
            client_name[0] = '\0';
        }
        //Передача клиентом имени игрока
        if (strlen(client_name) == 0)
            strcat(client_name, default_name);
        client_name[29] = '\0';
        int err = write(client_socket, client_name, MAX_USER_NAME_LENGTH);
        if (err == -1) {
            perror("Error: 'Can`t send user name'\n");
            exit(1);
        }
    }else {
            fprintf(stderr, "Error read username");
            exit(1);
    }
 */
    return SUCCESS;
}

void client_quit(const int *client_socket) {
    struct _config_frame configFrame = {.function = CLIENT_QUIT, .function_parameter = 0};
    pack(*client_socket, &configFrame);
    printf("Close socket....................................");
    shutdown(*client_socket, SHUT_RDWR);
    close(*client_socket);
    printf("done\n");
}

void get_books_net(const int *client_socket, struct book ***books, int *lenght) {
    struct _config_frame configFrame = {.function = GET_ALL_BOOK, .function_parameter = 0};
    int old_lenght = *lenght;
    (*lenght) = 0;
    pack(*client_socket, &configFrame);
    struct _book_frame bf;
    while (true) {
        unpack_book(*client_socket, &bf);
        if (bf.function == SEND_BOOK_EOF) {
            (*books)[(*lenght)] = NULL;
            break;
        } else {
            if ((*books)[(*lenght)] == NULL) {
                (*books)[(*lenght)] = calloc(1, sizeof(struct book));
            }
            memcpy((*books)[(*lenght)], &(bf.book), sizeof(struct book));
            (*lenght)++;
            if (old_lenght <= *lenght)
                (*books) = realloc((*books), ((*lenght) + 1) * sizeof(struct book *));
        }
    }
}

void update_book(const int *client_socket, struct book *book) {
    struct _config_frame configFrame = {.function = CLIENT_UPDATE_BOOK, .function_parameter = 0};
    struct _book_frame bookFrame = {.function = CLIENT_UPDATE_BOOK, .book = *book};
    pack(*client_socket, &configFrame);
    pack_book(*client_socket, &bookFrame);
}