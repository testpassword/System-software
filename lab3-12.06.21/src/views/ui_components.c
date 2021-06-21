#include <string.h>
#include "../../include/views/ui_components.h"

int init_curses() {
    if (!initscr()) {
        println(stderr, "error init library ncurses which responsible for ui drawing");
        return 1;
    } else {
        cbreak();
        keypad(stdscr, TRUE);
        return 0;
    }
}

void draw_borders(struct InputArea *cons) {
    wborder(cons->border.border_book_list, '|', '|', '-', '-', ' ', '*', ' ', '*');
    wborder(cons->border.border_book_info, '|', '|', '-', '-', '*', ' ', '*', ' ');
}

void draw_search(struct InputArea *cons, int colsBookList) {
    cons->forms.search.window = derwin(cons->text_area.books_list.search, 1, colsBookList, 0, 0);
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

void draw_editor(struct InputArea *cons, int rowBookInfo, int colsBookInfo, int colsBookList) {
    cons->forms.edit.window = derwin(cons->text_area.editor.book_info, rowBookInfo - 2, colsBookInfo, 1, 1);
    cons->forms.edit.fields = calloc(2, sizeof(FIELD *));
    cons->forms.edit.fields[0] = new_field(5, colsBookInfo-4, 2, colsBookList+2, 5, 0);
    cons->forms.edit.fields[1] = NULL;
    set_field_buffer(cons->forms.edit.fields[0], 0, "");
    set_field_opts(cons->forms.edit.fields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    set_field_back(cons->forms.edit.fields[0], A_UNDERLINE);
    cons->forms.edit.form = new_form(cons->forms.edit.fields);
    set_form_win(cons->forms.edit.form, cons->forms.edit.window);
}

int init(struct InputArea *cons) {
    if (init_curses()) return 1;
    int colsBookList = COLS * 2 / 10;
    int colsBookInfo = COLS * 8 / 10;
    int linesInBook = LINES - 3;
    cons->border.border_book_list = newwin(linesInBook, colsBookList, 1, 0);
    cons->border.border_book_info = newwin(linesInBook, colsBookInfo, 1, colsBookList);
    cons->text_area.books_list.book_list = newwin(linesInBook - 2, colsBookList - 4, 2, 2);
    cons->text_area.books_list.book_info = newwin(linesInBook - 2, colsBookInfo - 4, 2, colsBookList + 2);
    cons->text_area.books_list.book_lines = linesInBook - 2;
    cons->text_area.books_list.book_title_lenght = colsBookList - 8;
    cons->text_area.books_list.search = newwin(1, colsBookList, 0, 0);
    cons->text_area.books_list.top_btn = newwin(1, colsBookInfo - 2, 0, colsBookList + 2);
    cons->text_area.books_list.bottom_btn = newwin(1, COLS, LINES - 2, 0);
    cons->text_area.editor.book_list = cons->text_area.books_list.book_list;
    cons->text_area.editor.book_info = cons->text_area.books_list.book_info;
    cons->text_area.editor.top_btn = cons->text_area.books_list.top_btn;
    cons->text_area.editor.bottom_btn = cons->text_area.books_list.bottom_btn;
    draw_search(cons, colsBookList);
    draw_editor(cons, linesInBook, colsBookInfo, colsBookList);
    return 0;
}

void redraw_borders(struct InputArea *cons) {
    draw_borders(cons);
    refresh();
    wrefresh(cons->border.border_book_list);
    wrefresh(cons->border.border_book_info);
}

void redraw_editor(struct InputArea *cons) {
    wborder(cons->border.border_book_list, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(cons->forms.edit.window, '|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    wrefresh(cons->border.border_book_list);
    wrefresh(cons->forms.edit.window);
    wrefresh(cons->text_area.books_list.book_info);
    wrefresh(cons->text_area.books_list.book_list);
    wrefresh(cons->text_area.books_list.top_btn);
    wrefresh(cons->text_area.books_list.bottom_btn);
    refresh();
}

void redraw_book_list(struct InputArea *cons){
    redraw_borders(cons);
    wrefresh(cons->text_area.books_list.book_info);
    wrefresh(cons->text_area.books_list.book_list);
    wrefresh(cons->text_area.books_list.search);
    wrefresh(cons->text_area.books_list.top_btn);
    wrefresh(cons->text_area.books_list.bottom_btn);
    wrefresh(cons->forms.search.window);
    refresh();
}

void clear_TextArea(struct TextArea *textArea) {
    delwin(textArea->books_list.book_info);
    delwin(textArea->books_list.book_list);
    delwin(textArea->books_list.search);
    delwin(textArea->books_list.top_btn);
    delwin(textArea->books_list.bottom_btn);
}

void clear_Border(struct Border *boxArea) {
    delwin(boxArea->border_book_info);
    delwin(boxArea->border_book_list);
}

void clear_Editor(struct Form *search) {
    delwin(search->window);
    free_form(search->form);
    free_field(search->fields[0]);
    free(search->fields);
}

void clear_search(struct Form *search) {
    delwin(search->window);
    free_form(search->form);
    free_field(search->fields[0]);
    free_field(search->fields[1]);
    free(search->fields);
}

void clear_forms(struct Bar *forms) {
    clear_search(&(forms->search));
    clear_Editor(&(forms->edit));
}

void clear_InputArea(struct InputArea *cons) {
    clear_forms(&(cons->forms));
    clear_Border(&(cons->border));
    clear_TextArea(&(cons->text_area));
}

void destroy(struct InputArea *cons) {
    clear_InputArea(cons);
    endwin();
}

void draw_book_list(struct InputArea *cons, struct Book **books, int lenght, int selectedPage, int selectedBook) {
    wclear(cons->text_area.books_list.book_list);
    for (int i = 0; i < cons->text_area.books_list.book_lines; i++) {
        int index = cons->text_area.books_list.book_lines * selectedPage + i;
        if (index >= lenght) { break; }
        struct Book *bk = books[index];
        if (bk == NULL) continue;
        char *m = calloc(cons->text_area.books_list.book_title_lenght + 1, sizeof(char));
        memcpy(m, bk->title, cons->text_area.books_list.book_title_lenght);
        if (i == selectedBook) wprintw(cons->text_area.books_list.book_list, "> %s\n", m);
        else wprintw(cons->text_area.books_list.book_list, "%s\n", m);
        free(m);
    }
}

void draw_book_info(struct InputArea *cons, struct Book *book) {
    wclear(cons->text_area.books_list.book_info);
    wprintw(cons->text_area.books_list.book_info, "Title: %s\n", book->title);
    wprintw(cons->text_area.books_list.book_info, "Author: %s\n", book->authors);
    wprintw(cons->text_area.books_list.book_info, "Annotation: %s\n", book->annotation);
    wprintw(cons->text_area.books_list.book_info, "Tag: %s\n", book->tags);
    wprintw(cons->text_area.books_list.book_info, "Available: %d\n", book->available);
}

void draw_upper_key_tips(struct InputArea *cons, const bool *open_edit_form) {
    wclear(cons->text_area.books_list.top_btn);
    if(!(*open_edit_form)) {
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F1");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " get | ");
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F2");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " return | ");
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F5");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " edit | ");
    } else {
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F1");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " Title | ");
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F2");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " Author | ");
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F3");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " Annotation | ");
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F4");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " Tag | ");
        wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, "F5");
        wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.top_btn, " save | ");
    }
    wattron(cons->text_area.books_list.top_btn, A_UNDERLINE);
    wprintw(cons->text_area.books_list.top_btn, "F10");
    wattroff(cons->text_area.books_list.top_btn, A_UNDERLINE);
    wprintw(cons->text_area.books_list.top_btn, " exit ");
}

void draw_bottom_key_tips(struct InputArea *cons, const bool *checkboxFilter, const bool *open_edit_form, const int *editField) {
    wclear(cons->text_area.books_list.bottom_btn);
    if(!(*open_edit_form)) {
        wprintw(cons->text_area.books_list.bottom_btn, "filter by: ");
        wattron(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, "F6");
        wattroff(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, " Title%s", checkboxFilter[CHECKBOX_FILTER_BY_TITLE] ? "[X]" : "[ ]");
        wprintw(cons->text_area.books_list.bottom_btn, " | ");
        wattron(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, "F7");
        wattroff(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, " Author%s", checkboxFilter[CHECKBOX_FILTER_BY_AUTHOR] ? "[X]" : "[ ]");
        wprintw(cons->text_area.books_list.bottom_btn, " | ");
        wattron(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, "F8");
        wattroff(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, " Annotation%s", checkboxFilter[CHECKBOX_FILTER_BY_ANNOTATION] ? "[X]" : "[ ]");
        wprintw(cons->text_area.books_list.bottom_btn, " | ");
        wattron(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, "F9");
        wattroff(cons->text_area.books_list.bottom_btn, A_UNDERLINE);
        wprintw(cons->text_area.books_list.bottom_btn, " Tag%s", checkboxFilter[CHECKBOX_FILTER_BY_TAG] ? "[X]" : "[ ]");
        wprintw(cons->text_area.books_list.bottom_btn, " ");
    } else {
        wprintw(cons->text_area.books_list.bottom_btn, "Edit Book ");
        switch (*editField) {
            case EDIT_BOX_TITLE:
                wprintw(cons->text_area.books_list.bottom_btn, "Title");
                break;
            case EDIT_BOX_AUTHOR:
                wprintw(cons->text_area.books_list.bottom_btn, "Author");
                break;
            case EDIT_BOX_ANNOTATION:
                wprintw(cons->text_area.books_list.bottom_btn, "Annotation");
                break;
            case EDIT_BOX_TAG:
                wprintw(cons->text_area.books_list.bottom_btn, "Tag");
                break;
            case EDIT_BOX_NONE:
            default:
                break;
        }
    }
}

void draw_choosen_book_info(struct InputArea *cons, struct Book **books, int selectBook, const int *editField) {
    if (*editField == EDIT_BOX_NONE) draw_book_info(cons, (struct Book*) books[selectBook]);
}

void redraw(size_t *args) {
    struct InputArea *cons = (struct InputArea *) args[0];
    struct Book **books= (struct Book **) args[1];
    int selectedPage = *((int *)args[2]);
    int selectedBook = *((int *)args[3]);
    int count_book = *((int *)args[4]);
    bool *open_edit_field = (bool *) args[5];
    bool *checkboxFilter = (bool *) args[6];
    int *editField = (int *) args[8];
    int selectBook = selectedPage * cons->text_area.books_list.book_lines + selectedBook;
    draw_upper_key_tips(cons, open_edit_field);
    draw_bottom_key_tips(cons, checkboxFilter, open_edit_field, editField);
    draw_book_list(cons, books, count_book, selectedPage, selectedBook);
    if (*open_edit_field) {
        draw_choosen_book_info(cons, books, selectBook, editField);
        redraw_editor(cons);
    } else {
        draw_book_info(cons, books[selectBook]);
        redraw_book_list(cons);
    }
}

void clear_window(size_t *args) {
    struct InputArea* cons = (struct InputArea *) args[0];
    wclear(cons->text_area.books_list.book_info);
    wclear(cons->border.border_book_info);
    wclear(cons->border.border_book_list);
    wclear(cons->forms.edit.window);
}