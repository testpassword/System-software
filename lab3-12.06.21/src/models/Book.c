#include <stdio.h>
#include "../../include/models/Book.h"

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