#ifndef SPO_LAB3_BOOK_H
#define SPO_LAB3_BOOK_H
#include "stdlib.h"
#define MAX_BOOK_TITLE_LENGTH 256
#define MAX_BOOK_AUTHORS_AMOUNT 32
#define MAX_BOOK_ANNOTATION_LENGTH 512
#define MAX_BOOK_AUTHOR_NAME_LENGTH 32
#define MAX_BOOK_TAGS_AMOUNT 16
#define MAX_BOOK_TAG_LENGTH 32

struct Book {
    size_t bookID;
    char title[MAX_BOOK_TITLE_LENGTH];
    char authors[MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH];
    char annotation[MAX_BOOK_ANNOTATION_LENGTH];
    char tags[MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH];
    char available;
};

struct Book** generate_books(size_t lenght);
void burn_books(struct Book **books, const int count_book);
struct Book** read_book();
int count_books(struct Book **books);
void get_books_net(const int* client_socket, struct Book *** books, int* lenght);
void update_book(const int* client_socket, struct Book* book);
#endif
