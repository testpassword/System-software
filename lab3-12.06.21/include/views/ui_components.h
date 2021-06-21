#ifndef SPO_LAB3_UI_H
#define SPO_LAB3_UI_H

#include <curses.h>
#include <form.h>
#include "../models/Book.h"

#define CHECKBOX_FILTER_BY_TITLE 0
#define CHECKBOX_FILTER_BY_AUTHOR 1
#define CHECKBOX_FILTER_BY_ANNOTATION 2
#define CHECKBOX_FILTER_BY_TAG 3
#define EDIT_BOX_TITLE 0
#define EDIT_BOX_AUTHOR 1
#define EDIT_BOX_ANNOTATION 2
#define EDIT_BOX_TAG 3
#define EDIT_BOX_NONE -1

struct Border {
    WINDOW *border_book_list;
    WINDOW *border_book_info;
};

struct Editor {
    WINDOW *top_btn;
    WINDOW *book_list;
    WINDOW *book_info;
    WINDOW *bottom_btn;
};

struct BookList {
    WINDOW *search;
    WINDOW *top_btn;
    WINDOW *book_list;
    WINDOW *book_info;
    WINDOW *bottom_btn;
    int book_lines;
    int book_title_lenght;
};

struct TextArea {
    struct BookList books_list;
    struct Editor editor;
};

struct Form {
    WINDOW *window;
    FORM *form;
    FIELD **fields;
};

struct Bar {
    struct Form search;
    struct Form edit;
};

struct InputArea {
    struct Border border;
    struct TextArea text_area;
    struct Bar forms;
};

int init(struct InputArea *cons);
void destroy(struct InputArea *cons);
void redraw(size_t *args);
void clear_window(size_t *args);

#endif