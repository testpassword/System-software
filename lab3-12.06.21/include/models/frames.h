#ifndef SPO_LAB3_FRAME_H
#define SPO_LAB3_FRAME_H
#include "Book.h"
#define MAX_CLIENT_NUMBER 8
#ifndef __USE_PROTOCOL
#define SERVER_FULL  1
#define GET_ALL_BOOK 2
#define SEND_BOOK_EOF 3
#define SEND_BOOK 4
#define CLIENT_QUIT 5
#define CLIENT_UPDATE_BOOK 6
#define SEND_UPDATE_INFO 7
#define SERVER_QUIT 8
#endif

struct Frame {
    unsigned char function;
    unsigned char function_parameter;
};

struct BookFrame {
    unsigned char function;
    struct Book book;
};

int pack_frame(int socket, struct Frame *frame);
int unpack_frame(int socket, struct Frame *frame);
int pack_book(int socket, struct BookFrame *frame);
int unpack_book(int socket, struct BookFrame *frame);

#endif
