#include <stdio.h>
#include <stdlib.h>
#include "../../include/models/Book.h"
#include "../../include/controllers/client.h"
#include "../../include/dtos/Frame.h"

struct Book** generate_books(size_t count) {
    const char* names[6];
    names[0] = "Klimenkov"; names[1] = "Zopa"; names[2] = "Nikolaev";
    names[3] = "Afanasiev"; names[4] = "Pismak"; names[5] = "Gavrilov";
    const char* titles[9];
    titles[0] = "software testing"; titles[1] = "software engineering"; titles[2] = "web-programming";
    titles[3] = "programming"; titles[4] = "mobile application development"; titles[5] = "operating systems";
    titles[6] = "information systems and databases"; titles[7] = "computational math"; titles[8] = "algorithms and data structures";
    const char* tags[5];
    tags[0] = "fiction"; tags[1] = "monitor"; tags[2] = "amazing";
    tags[3] = "payback"; tags[4] = "water";
    const char* annotations[2];
    annotations[0] = "i am to lazy to write annotations"; annotations[1] = "now very hot on outside";
    struct Book** books = calloc(count + 1, sizeof(struct Book*));
    for (size_t i = 0; i < count; i++) {
        books[i] = calloc(1, sizeof(struct Book));
        books[i]->bookID = i;
        int count = rand() % 10;
        books[i]->max = count;
        books[i]->available = count;
        snprintf(books[i]->title, MAX_BOOK_TITLE_LENGTH, titles[rand() % 9]);
        snprintf(books[i]->authors, MAX_BOOK_AUTHORS_AMOUNT * MAX_BOOK_AUTHOR_NAME_LENGTH, names[rand() % 6]);
        snprintf(books[i]->tags, MAX_BOOK_TAGS_AMOUNT * MAX_BOOK_TAG_LENGTH, tags[rand() % 5]);
        snprintf(books[i]->annotation, MAX_BOOK_ANNOTATION_LENGTH, annotations[rand() % 2]);
    }
    books[count] = NULL;
    return books;
}

int count_books(struct Book **books) { for (int i = 0; ; i++) if (books[i] == NULL) return i + 1; }

void burn_books(struct Book **books, const int count) {
    for (int i = 0; i < count; i++) if (books[i]) free(books[i]);
    free(books);
}

struct Book** read_books() { return generate_books(15); }

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
            if ((*books)[(*lenght)] == NULL) (*books)[(*lenght)] = calloc(1, sizeof(struct Book));
            memcpy((*books)[(*lenght)], &(bf.book), sizeof(struct Book));
            (*lenght)++;
            if (old_lenght <= *lenght) (*books) = realloc((*books), ((*lenght) + 1) * sizeof(struct Book *));
        }
    }
}

void update_book(const int *client_socket, struct Book *book) {
    pack_frame(*client_socket, &(struct Frame) {.function = CLIENT_UPDATE_BOOK, .function_parameter = 0});
    pack_book(*client_socket, &(struct BookFrame) {.function = CLIENT_UPDATE_BOOK, .book = *book});
}