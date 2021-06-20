#include <stdio.h>
#include "../../include/models/Book.h"
#include "../../include/controllers/client.h"
#include "../../include/dtos/Frame.h"

struct Book** generate_books(size_t lenght) {
    struct Book** books = calloc(lenght + 1, sizeof(struct Book*));
    for (size_t i = 0; i < lenght; i++) {
        books[i] = calloc(1, sizeof(struct Book));
        books[i]->bookID = i;
        books[i]->available = 10;
        snprintf(books[i]->title, MAX_BOOK_TITLE_LENGTH, "Book%zu", i);
        snprintf(books[i]->authors, MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH, "Author1, Author2");
        snprintf(books[i]->tags, MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH, "Cool");
        snprintf(books[i]->annotation, MAX_BOOK_ANNOTATION_LENGTH, "I Billy Harrington!");
    }
    books[lenght] = NULL;
    return books;
}

int count_books(struct Book **books) { for (int i = 0; ; i++) if (books[i] == NULL) return i + 1; }

void burn_books(struct Book **books, const int count_book) {
    for (int i = 0; i < count_book; i++) if (books[i]) free(books[i]);
    free(books);
}

struct Book** read_book() { return generate_books(15); }

void get_books_net(const int *client_socket, struct Book ***books, int *lenght) {
    int old_lenght = *lenght;
    (*lenght) = 0;
    pack_frame(*client_socket, &(struct Frame) {.function = GET_ALL_BOOK, .function_parameter = 0});
    struct BookFrame bf;
    while (true) {
        unpack_book(*client_socket, &bf);
        if (bf.function == SEND_BOOK_EOF) {
            (*books)[(*lenght)] = NULL;
            break;
        } else {
            if ((*books)[(*lenght)] == NULL)
                (*books)[(*lenght)] = calloc(1, sizeof(struct Book));
            memcpy((*books)[(*lenght)], &(bf.book), sizeof(struct Book));
            (*lenght)++;
            if (old_lenght <= *lenght)
                (*books) = realloc((*books), ((*lenght) + 1) * sizeof(struct Book *));
        }
    }
}

void update_book(const int *client_socket, struct Book *book) {
    pack_frame(*client_socket, &(struct Frame) {.function = CLIENT_UPDATE_BOOK, .function_parameter = 0});
    pack_book(*client_socket, &(struct BookFrame) {.function = CLIENT_UPDATE_BOOK, .book = *book});
}