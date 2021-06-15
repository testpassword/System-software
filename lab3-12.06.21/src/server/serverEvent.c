#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include "../../include/server/server.h"
#include "../../include/server/serverEvent.h"

bool com_default(size_t *args) {
    size_t i = args[0];
    struct _client_socket *client_sockets = (struct _client_socket *) args[1];
    fprintf(stderr, "Client #%zu: Unknown packet\n", i);
    close(client_sockets[i].client_socket);
    client_sockets[i].active = false;
    return true;
}

bool com_get_all_book(size_t *args) {
    size_t i = args[0];
    struct _client_socket *client_sockets = (struct _client_socket *) args[1];
    size_t lenght_book = args[2];
    struct book **books = (struct book **) args[3];
    printf("Client #%zu: Get All\n", i);
    struct _book_frame bookFrame;
    for (int lbook = 0; lbook < lenght_book; lbook++) {
        if (books[lbook]) {
            bookFrame.book = *(books[lbook]);
            bookFrame.function = SEND_BOOK;
        } else {
            bookFrame.book = (struct book) {};
            bookFrame.function = SEND_BOOK_EOF;
        }
        pack_book(client_sockets[i].client_socket, &bookFrame);
    }
    return false;
}

bool com_client_quit(size_t *args) {
    size_t i = args[0];
    struct _client_socket *client_sockets = (struct _client_socket *) args[1];
    printf("Client #%zu: Quit\n", i);
    close(client_sockets[i].client_socket);
    client_sockets[i].active = false;
    return true;
}

void packAll(const int *client_number, struct _client_socket *client_sockets, struct _config_frame *configFrame) {
    for (int i = 0; i < *client_number; i++)
        if (client_sockets[i].client_socket > 0 && client_sockets[i].active)
            pack(client_sockets[i].client_socket, configFrame);
}

bool com_client_update_book(size_t *args) {
    size_t i = args[0];
    struct _client_socket *client_sockets = (struct _client_socket *) args[1];
    size_t lenght_book = args[2];
    struct book **books = (struct book **) args[3];
    int *client_number = (int *) args[4];
    printf("Client #%zu: update Book\n", i);
    struct _book_frame bookFrameUpdate;
    unpack_book(client_sockets[i].client_socket, &bookFrameUpdate);
    for (int lbook = 0; lbook < lenght_book; lbook++)
        if (books[lbook]->bookID == bookFrameUpdate.book.bookID) {
            memcpy(books[lbook], &(bookFrameUpdate.book), sizeof(struct book));
            break;
        }
    struct _config_frame configFrame;
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