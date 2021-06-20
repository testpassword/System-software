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
    WINDOW *borderBookListW;
    WINDOW *borderBookInfoW;
};

struct Editor {
    WINDOW *topButtonW;
    WINDOW *bookListW;
    WINDOW *bookInfoW;
    WINDOW *bottomButtonW;
};

struct BookList {
    WINDOW *searchW;
    WINDOW *topButtonW;
    WINDOW *bookListW;
    WINDOW *bookInfoW;
    WINDOW *bottomButtonW;
    int bookWLines;
    int bookNameLenght;
};

struct TextArea {
    struct BookList mainWindow;
    struct Editor editWindow;
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
    struct TextArea textArea;
    struct Bar forms;
};

int init(struct InputArea *cons);
void destroy(struct InputArea *cons);
void redraw(size_t *args);
void clear_window(size_t *args);

#endif