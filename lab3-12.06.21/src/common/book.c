#include <stdio.h>
#include "../../include/common/book.h"

struct book** generate_books(size_t lenght) {
    struct book **books = calloc(lenght+1, sizeof(struct book *));
    for (size_t i = 0; i < lenght; i++) {
        books[i] = calloc(1, sizeof(struct book));
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

int get_lenght_book(struct book **books) {
    for (int i = 0; ; i++)
        if (books[i] == NULL) return i+1;
}

void free_books(struct book **books, const int count_book) {
    for (int i = 0; i < count_book; i++)
        if (books[i]) free(books[i]);
    free(books);
}

struct book** read_book() { return generate_books(15); }