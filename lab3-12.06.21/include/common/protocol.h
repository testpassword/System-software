#ifndef SPO_LAB3_PROTOCOL_H
#define SPO_LAB3_PROTOCOL_H

#include "book.h"

//#define SERVER_TCP_PORT 1404
#define MAX_CLIENT_NUMBER 8

/// Протокол связи и сообщения
#ifndef __USE_PROTOCOL
#define SERVER_FULL  1  // Сервер передаёт клиенту информацию о удачном подключении или причину отказа
#define GET_ALL_BOOK 2
#define SEND_BOOK_EOF 3
#define SEND_BOOK 4
#define CLIENT_QUIT 5
#define CLIENT_UPDATE_BOOK 6
#define SEND_UPDATE_INFO 7
#define SERVER_QUIT 8
#endif

/// Стандартный кадр (2 байта)
struct _config_frame {
    unsigned char function;
    unsigned char function_parameter;
};

struct _book_frame {
    unsigned char function;
    struct book book;
};

/// Запаковка стандартного кадра и отправка
int pack(int socket, struct _config_frame *frame);
/// Распаковка стандартного кадра
int unpack(int socket, struct _config_frame *frame);

int pack_book(int socket, struct _book_frame *frame);
int unpack_book(int socket, struct _book_frame *frame);

#endif //SPO_LAB3_PROTOCOL_H
