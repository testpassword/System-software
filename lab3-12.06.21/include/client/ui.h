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

struct border {
    WINDOW *borderBookListW;
    WINDOW *borderBookInfoW;
};

struct editWindows {
    WINDOW *topButtonW;
    WINDOW *bookListW;
    WINDOW *bookInfoW;
    WINDOW *bottomButtonW;
};

struct mainWindow {
    WINDOW *searchW;
    WINDOW *topButtonW;
    WINDOW *bookListW;
    WINDOW *bookInfoW;
    WINDOW *bottomButtonW;
    int bookWLines;
    int bookNameLenght;
};

struct textArea {
    struct mainWindow mainWindow;
    struct editWindows editWindow;
};

struct form {
    WINDOW *window;
    FORM *form;
    FIELD **fields;
};

struct forms {
    struct form search;
    struct form edit;
};

struct console {
    struct border border;
    struct textArea textArea;
    struct forms forms;
};

int initUI(struct console *cons);
void closeUI(struct console *cons);
//void refreshMainWindow(struct console *cons);

//void printBookInfo(struct console *cons, struct Book *Book);
//void printBookList(struct console *cons, struct Book **books, int lenght, int selectedPage, int selectedBook);
//void printTopMenu(struct console *cons);
//void printBottonMenu(struct console *cons, const bool *checkboxFilter);
void update(size_t *args);
void clearAllWindow(size_t *args);

#endif //SPO_LAB3_UI_H
