#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../../include/dtos/BookFrame.h"
#include "../../include/errors.h"
#include "../../include/dtos/Frame.h"
#include "../../include/str_extensions.h"

int pack_book(int socket, struct BookFrame *frame) {
    if (send(socket, frame, sizeof(struct BookFrame), 0) == -1) {
        println("\nError: 'send message'");
        return ERR_SERVER_SEND_BOOK;
    }
    return 0;
}

int unpack_book(int socket, struct BookFrame *frame) {
    if (read(socket, frame, sizeof(struct BookFrame)) < 1) {
        println("Error read in unpack_frame");
        exit(1);
    }
    return 0;
}
