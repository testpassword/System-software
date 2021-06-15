#ifndef SPO_LAB3_BOOK_H
#define SPO_LAB3_BOOK_H

#include "stdlib.h"

#define MAX_BOOKS_AMOUNT 500
#define MAX_BOOK_TITLE_LENGTH 256
#define MAX_BOOK_AUTHORS_AMOUNT 32
#define MAX_BOOK_ANNOTATION_LENGTH 512
#define MAX_BOOK_AUTHOR_NAME_LENGTH 32
#define MAX_BOOK_TAGS_AMOUNT 16
#define MAX_BOOK_TAG_LENGTH 32

struct book {
    size_t bookID;
    char title[MAX_BOOK_TITLE_LENGTH];
    char authors[MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH];
    char annotation[MAX_BOOK_ANNOTATION_LENGTH];
    char tags[MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH];
    char available;
};

struct book** generate_books(size_t lenght);
void free_books(struct book **books, const int count_book);
struct book** read_book();
int get_lenght_book(struct book **books);

#endif //SPO_LAB3_BOOK_H
