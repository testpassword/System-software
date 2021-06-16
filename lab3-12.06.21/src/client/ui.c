#include <string.h>
#include "../../include/client/ui.h"

int init_curses() {
    if (!initscr()) {
        fprintf(stderr, "Error initialising ncurses.\n");
        return 1;
    }
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    return 0;
}

void borderBook(struct console *cons) {
    wborder(cons->border.borderBookListW, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(cons->border.borderBookInfoW, '|', '|', '-', '-', '+', '+', '+', '+');
}

void initFormSearch(struct console *cons, int colsBookList) {
    cons->forms.search.window = derwin(cons->textArea.mainWindow.searchW, 1, colsBookList, 0, 0);
    cons->forms.search.fields = calloc(3, sizeof(FIELD *));
    cons->forms.search.fields[0] = new_field(1, colsBookList-2, 0, 2, 2, 0);
    cons->forms.search.fields[1] = new_field(1, 2, 0, 0, 0, 0);
    cons->forms.search.fields[2] = NULL;
    set_field_buffer(cons->forms.search.fields[0], 0, "");
    set_field_buffer(cons->forms.search.fields[1], 0, " >");
    set_field_opts(cons->forms.search.fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_opts(cons->forms.search.fields[1], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    cons->forms.search.form = new_form(cons->forms.search.fields);
    set_form_win(cons->forms.search.form, cons->forms.search.window);
    post_form(cons->forms.search.form);
}

void initFormEdit(struct console *cons, int rowBookInfo, int colsBookInfo,int colsBookList) {
    cons->forms.edit.window = derwin(cons->textArea.editWindow.bookInfoW, rowBookInfo-2, colsBookInfo, 1, 1);
    cons->forms.edit.fields = calloc(2, sizeof(FIELD *));
    cons->forms.edit.fields[0] = new_field(5, colsBookInfo-4, 2, colsBookList+2, 5, 0);
    cons->forms.edit.fields[1] = NULL;
    set_field_buffer(cons->forms.edit.fields[0], 0, "");
    set_field_opts(cons->forms.edit.fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_back(cons->forms.edit.fields[0], A_UNDERLINE);
    cons->forms.edit.form = new_form(cons->forms.edit.fields);
    set_form_win(cons->forms.edit.form, cons->forms.edit.window);
}

int initUI(struct console *cons) {
    if (init_curses()) return 1;
    int colsBookList = COLS * 2 / 10;
    int colsBookInfo = COLS * 8 / 10;
    int linesInBook = LINES - 3;
    cons->border.borderBookListW = newwin(linesInBook, colsBookList, 1, 0);
    cons->border.borderBookInfoW = newwin(linesInBook, colsBookInfo, 1, colsBookList);
    cons->textArea.mainWindow.bookListW = newwin(linesInBook - 2, colsBookList - 4, 2, 2);
    cons->textArea.mainWindow.bookInfoW = newwin(linesInBook - 2, colsBookInfo - 4, 2, colsBookList + 2);
    cons->textArea.mainWindow.bookWLines = linesInBook - 2;
    cons->textArea.mainWindow.bookNameLenght = colsBookList - 8;
    cons->textArea.mainWindow.searchW = newwin(1, colsBookList, 0, 0);
    cons->textArea.mainWindow.topButtonW = newwin(1, colsBookInfo - 2, 0, colsBookList + 2);
    cons->textArea.mainWindow.bottomButtonW = newwin(1, COLS, LINES - 2, 0);
    cons->textArea.editWindow.bookListW = cons->textArea.mainWindow.bookListW;
    cons->textArea.editWindow.bookInfoW = cons->textArea.mainWindow.bookInfoW;
    cons->textArea.editWindow.topButtonW = cons->textArea.mainWindow.topButtonW;
    cons->textArea.editWindow.bottomButtonW = cons->textArea.mainWindow.bottomButtonW;

//    immedok(cons->textArea.bookListW, TRUE);

    initFormSearch(cons, colsBookList);
    initFormEdit(cons, linesInBook, colsBookInfo, colsBookList);
    return 0;
}

void refreshBorder(struct console *cons) {
    borderBook(cons);
    refresh();
    wrefresh(cons->border.borderBookListW);
    wrefresh(cons->border.borderBookInfoW);
}

void refreshEditWindow(struct console *cons) {
    wborder(cons->border.borderBookListW, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(cons->forms.edit.window, '|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    wrefresh(cons->border.borderBookListW);
//    form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
//    form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
    wrefresh(cons->forms.edit.window);
    wrefresh(cons->textArea.mainWindow.bookInfoW);
    wrefresh(cons->textArea.mainWindow.bookListW);
    wrefresh(cons->textArea.mainWindow.topButtonW);
    wrefresh(cons->textArea.mainWindow.bottomButtonW);
    refresh();
}

void refreshMainWindow(struct console *cons){
    refreshBorder(cons);
    wrefresh(cons->textArea.mainWindow.bookInfoW);
    wrefresh(cons->textArea.mainWindow.bookListW);
    wrefresh(cons->textArea.mainWindow.searchW);
    wrefresh(cons->textArea.mainWindow.topButtonW);
    wrefresh(cons->textArea.mainWindow.bottomButtonW);
    wrefresh(cons->forms.search.window);
    refresh();
}

void delWindowTextArea(struct textArea *textArea) {
    delwin(textArea->mainWindow.bookInfoW);
    delwin(textArea->mainWindow.bookListW);
    delwin(textArea->mainWindow.searchW);
    delwin(textArea->mainWindow.topButtonW);
    delwin(textArea->mainWindow.bottomButtonW);
}

void delWindowBoxArea(struct border *boxArea) {
    delwin(boxArea->borderBookInfoW);
    delwin(boxArea->borderBookListW);
}

void delFromEdit(struct form *search) {
    delwin(search->window);
    free_form(search->form);
    free_field(search->fields[0]);
    free(search->fields);
}

void delFromSearch(struct form *search) {
    delwin(search->window);
    free_form(search->form);
    free_field(search->fields[0]);
    free_field(search->fields[1]);
    free(search->fields);
}

void delFroms(struct forms *forms) {
    delFromSearch(&(forms->search));
    delFromEdit(&(forms->edit));
}

void delWindow(struct console *cons) {
    delFroms(&(cons->forms));
    delWindowBoxArea(&(cons->border));
    delWindowTextArea(&(cons->textArea));
}

void closeUI(struct console *cons) {
    delWindow(cons);
    endwin();
}

void printBookList(struct console *cons, struct Book **books, int lenght, int selectedPage, int selectedBook) {
    wclear(cons->textArea.mainWindow.bookListW);
    for (int i = 0; i < cons->textArea.mainWindow.bookWLines; i++) {
        int index = cons->textArea.mainWindow.bookWLines * selectedPage + i;
        if (index >= lenght) { break; }
        struct Book *bk = books[index];
        if (bk == NULL) continue;
        char *m = calloc(cons->textArea.mainWindow.bookNameLenght+1, sizeof(char));
        memcpy(m, bk->title, cons->textArea.mainWindow.bookNameLenght);
        if (i == selectedBook) wprintw(cons->textArea.mainWindow.bookListW, "> %s\n", m);
        else wprintw(cons->textArea.mainWindow.bookListW, "%s\n", m);
        free(m);
    }
}

void printBookInfo(struct console *cons, struct Book *book) {
    wclear(cons->textArea.mainWindow.bookInfoW);
    wprintw(cons->textArea.mainWindow.bookInfoW, "Title: %s\n", book->title);
    wprintw(cons->textArea.mainWindow.bookInfoW, "Authors: %s\n", book->authors);
    wprintw(cons->textArea.mainWindow.bookInfoW, "Annotation: %s\n", book->annotation);
    wprintw(cons->textArea.mainWindow.bookInfoW, "Tags: %s\n", book->tags);
    wprintw(cons->textArea.mainWindow.bookInfoW, "Available: %d\n", book->available);
}

void printTopMenu(struct console *cons, const bool *open_edit_form) {
    wclear(cons->textArea.mainWindow.topButtonW);
    if(!(*open_edit_form)) {
        wprintw(cons->textArea.mainWindow.topButtonW, "[Get Book ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F1");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
        wprintw(cons->textArea.mainWindow.topButtonW, "[Return Book ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F2");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");

//        wprintw(cons->textArea.mainWindow.topButtonW, "[Add new Book ");
//        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
//        wprintw(cons->textArea.mainWindow.topButtonW, "F3");
//        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
//        wprintw(cons->textArea.mainWindow.topButtonW, "] ");

        wprintw(cons->textArea.mainWindow.topButtonW, "[To Edit Book ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F5");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
    } else {
        wprintw(cons->textArea.mainWindow.topButtonW, "[Title ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F1");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
        wprintw(cons->textArea.mainWindow.topButtonW, "[Authors ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F2");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
        wprintw(cons->textArea.mainWindow.topButtonW, "[Annotation ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F3");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
        wprintw(cons->textArea.mainWindow.topButtonW, "[Tags ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F4");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
        wprintw(cons->textArea.mainWindow.topButtonW, "[Return ");
        wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "F5");
        wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.topButtonW, "] ");
    }

    wprintw(cons->textArea.mainWindow.topButtonW, "[Exit ");
    wattron(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
    wprintw(cons->textArea.mainWindow.topButtonW, "F10");
    wattroff(cons->textArea.mainWindow.topButtonW, A_UNDERLINE);
    wprintw(cons->textArea.mainWindow.topButtonW, "]");
}

void printBottonMenu(struct console *cons, const bool *checkboxFilter, const bool *open_edit_form, const int *editField) {
    wclear(cons->textArea.mainWindow.bottomButtonW);
    if(!(*open_edit_form)) {
        wprintw(cons->textArea.mainWindow.bottomButtonW, "Filter by ");
        wprintw(cons->textArea.mainWindow.bottomButtonW, "%s Title (",
                checkboxFilter[CHECKBOX_FILTER_BY_TITLE] ? "[X]" : "[ ]");
        wattron(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "F6");
        wattroff(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "), ");
        wprintw(cons->textArea.mainWindow.bottomButtonW, "%s Author (",
                checkboxFilter[CHECKBOX_FILTER_BY_AUTHOR] ? "[X]" : "[ ]");
        wattron(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "F7");
        wattroff(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "), ");
        wprintw(cons->textArea.mainWindow.bottomButtonW, "%s Annotation (",
                checkboxFilter[CHECKBOX_FILTER_BY_ANNOTATION] ? "[X]" : "[ ]");
        wattron(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "F8");
        wattroff(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "), ");
        wprintw(cons->textArea.mainWindow.bottomButtonW, "%s Tag (",
                checkboxFilter[CHECKBOX_FILTER_BY_TAG] ? "[X]" : "[ ]");
        wattron(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, "F9");
        wattroff(cons->textArea.mainWindow.bottomButtonW, A_UNDERLINE);
        wprintw(cons->textArea.mainWindow.bottomButtonW, ") ");
    } else {
        wprintw(cons->textArea.mainWindow.bottomButtonW, "Edit Book ");
        switch (*editField) {
            case EDIT_BOX_TITLE:
                wprintw(cons->textArea.mainWindow.bottomButtonW, "Title");
                break;
            case EDIT_BOX_AUTHOR:
                wprintw(cons->textArea.mainWindow.bottomButtonW, "Author");
                break;
            case EDIT_BOX_ANNOTATION:
                wprintw(cons->textArea.mainWindow.bottomButtonW, "Annotation");
                break;
            case EDIT_BOX_TAG:
                wprintw(cons->textArea.mainWindow.bottomButtonW, "Tag");
                break;
            case EDIT_BOX_NONE:
            default:
                break;
        }
    }
}

void printSelectedBook(struct console *cons, struct Book **books, int selectBook, const int *editField) {
    struct Book *bk = books[selectBook];
    if (*editField == EDIT_BOX_NONE) printBookInfo(cons, bk);
}

void update(size_t *args) {
    struct console *cons = (struct console *) args[0];
    struct Book **books= (struct Book **) args[1];
    int selectedPage = *((int *)args[2]);
    int selectedBook = *((int *)args[3]);
    int count_book = *((int *)args[4]);
    bool *open_edit_field = (bool *) args[5];
    bool *checkboxFilter = (bool *) args[6];
    int *editField = (int *) args[8];
    int selectBook = selectedPage * cons->textArea.mainWindow.bookWLines + selectedBook;
    printTopMenu(cons, open_edit_field);
    printBottonMenu(cons, checkboxFilter, open_edit_field, editField);
    printBookList(cons, books, count_book, selectedPage, selectedBook);
    if(*open_edit_field) {
        printSelectedBook(cons, books, selectBook, editField);
        refreshEditWindow(cons);
    } else {
        printBookInfo(cons, books[selectBook]);
        refreshMainWindow(cons);
    }
}

void clearAllWindow(size_t *args) {
    struct console *cons = (struct console *) args[0];
    wclear(cons->textArea.mainWindow.bookInfoW);
    wclear(cons->border.borderBookInfoW);
    wclear(cons->border.borderBookListW);
    wclear(cons->forms.edit.window);
}