#include <curses.h>
#include <string.h>
#include "../../include/client/clientEvent.h"
#include "../../include/client/ui.h"
#include "../../include/common/utils.h"
#include "../../include/client/network.h"

void searchText(size_t *args) {
    struct console *cons = (struct console *) args[0];
    struct book **books = (struct book **) args[1];
    int *selectedPage = ((int *) args[2]);
    int *selectedBook = ((int *) args[3]);
    int *lenghBook = (int *) args[4];
    bool *checkbox = (bool *) args[6];
    form_driver(cons->forms.search.form, REQ_PREV_FIELD);
    form_driver(cons->forms.search.form, REQ_NEXT_FIELD);
    char *field_buffer_value = trim(field_buffer(cons->forms.search.fields[0], 0));
    for (int i = 0; i < *lenghBook; i++) {
        struct book *book = books[i];
        int t1 = checkbox[CHECKBOX_FILTER_BY_TITLE] ? seek_substring_KMP(book->title, field_buffer_value) : -1;
        int t2 = checkbox[CHECKBOX_FILTER_BY_TAG] ? seek_substring_KMP(book->tags, field_buffer_value) : -1;
        int a1 = checkbox[CHECKBOX_FILTER_BY_AUTHOR] ? seek_substring_KMP(book->authors, field_buffer_value) : -1;
        int a2 = checkbox[CHECKBOX_FILTER_BY_ANNOTATION] ? seek_substring_KMP(book->annotation, field_buffer_value) : -1;
        if (t1 == -1 && t2 == -1 && a1 == -1 && a2 == -1) continue;
        else {
            if (i >= cons->textArea.mainWindow.bookWLines) *selectedPage = i / cons->textArea.mainWindow.bookWLines;
            *selectedBook = i % cons->textArea.mainWindow.bookWLines;
            break;
        }
    }
}

bool cmd_symbol(size_t *args) {
    struct console *cons = (struct console *) args[0];
    bool *open_edit_from = (bool *) args[5];
    int *ch = (int *) args[7];
    int *editField = (int *) args[8];
    FORM *form = *open_edit_from ? cons->forms.edit.form : cons->forms.search.form;
    switch (*ch) {
        case KEY_BACKSPACE:
        case KEY_DEL:
            form_driver(form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(form, REQ_DEL_CHAR);
            break;
        case KEY_LEFT:
            form_driver(form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(form, REQ_NEXT_CHAR);
            break;
        case KEY_NL:
        case KEY_ENTER:
            if (!(*open_edit_from)) {
                searchText(args);
                *(bool *)(args[10]) = true;
            }
            break;
        default:
            if (*open_edit_from && *editField == EDIT_BOX_NONE) break;
            form_driver(form, *ch);
            break;
    }
    return true;
}

bool cmd_KEY_MOUSE(size_t *args) {
    MEVENT event;
    if (getmouse(&event) == OK) {}
    return true;
}

bool cmd_KEY_UP(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!(*open_edit_form)) {
        struct console *cons = (struct console *) args[0];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        (*selectedBook)--;
        if ((*selectedBook) < 0) {
            (*selectedPage)--;
            if ((*selectedPage) < 0) {
                (*selectedPage) = 0;
                (*selectedBook) = 0;
            } else (*selectedBook) = lenghtArea - 1;
        }
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        if (selectBook <= 0) (*selectedBook) = 0;
    }
    return true;
}

bool cmd_KEY_DOWN(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!(*open_edit_form)) {
        struct console *cons = (struct console *) args[0];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *lenght = (int *) args[4];
        int pages = (*lenght) / cons->textArea.mainWindow.bookWLines + 1;
        (*selectedBook)++;
        if ((*selectedBook) >= lenghtArea) {
            (*selectedPage)++;
            if ((*selectedPage) >= pages) {
                (*selectedPage) = pages - 1;
                (*selectedBook)--;
            } else (*selectedBook) = 0;
        }
        if ((*selectedBook) >= (*lenght)) (*selectedBook) = (*lenght) - 1;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        if (selectBook >= (*lenght)) {
            selectBook--;
            (*selectedBook)--;
        }
    }
    return true;
}

bool cmd_KEY_F1(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        struct book **books = (struct book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_TITLE;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->title);
    } else {
        struct console *cons = (struct console *) args[0];
        struct book **books = (struct book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        if (cur_book->available > 0) cur_book->available--;
        int *client_socket = (int *) args[9];
        update_book(client_socket, cur_book);
    }
    return true;
}

bool cmd_KEY_F2(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        struct book **books = (struct book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_AUTHOR;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->authors);
    } else {
        struct console *cons = (struct console *) args[0];
        struct book **books = (struct book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        if(cur_book->available < 10) cur_book->available++;
        int *client_socket = (int *) args[9];
        update_book(client_socket, cur_book);
    }
    return true;
}

bool cmd_KEY_F3(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        struct book **books = (struct book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_ANNOTATION;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->annotation);
    }
    return true;
}

bool cmd_KEY_F4(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct console *cons = (struct console *) args[0];
        struct book **books = (struct book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_NEXT_FIELD);
        form_driver(cons->forms.edit.form, REQ_PREV_FIELD);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_TAG;
        set_field_buffer(cons->forms.edit.fields[0], 0, cur_book->tags);
    }
    return true;
}

bool cmd_KEY_F5(size_t *args) {
    struct console *cons = (struct console *) args[0];
    bool *open_edit_form = (bool *) args[5];
    *open_edit_form = !(*open_edit_form);
    if (*open_edit_form) {
        unpost_form(cons->forms.search.form);
        post_form(cons->forms.edit.form);
        set_current_field(cons->forms.edit.form, cons->forms.edit.fields[0]);
    } else {
        struct book **books = (struct book **) args[1];
        int lenghtArea = cons->textArea.mainWindow.bookWLines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct book *cur_book = books[selectBook];
        form_driver(cons->forms.edit.form, REQ_VALIDATION);
        char *field_buffer_value = trim(field_buffer(cons->forms.edit.fields[0], 0));
        int *editField = (int *) args[8];
        switch (*editField) {
            case EDIT_BOX_TITLE:
                memcpy(cur_book->title, field_buffer_value, MAX_BOOK_TITLE_LENGTH);
                break;
            case EDIT_BOX_AUTHOR:
                memcpy(cur_book->authors, field_buffer_value, MAX_BOOK_AUTHORS_AMOUNT);
                break;
            case EDIT_BOX_ANNOTATION:
                memcpy(cur_book->annotation, field_buffer_value, MAX_BOOK_ANNOTATION_LENGTH);
                break;
            case EDIT_BOX_TAG:
                memcpy(cur_book->tags, field_buffer_value, MAX_BOOK_TAGS_AMOUNT);
                break;
            default:
                break;
        }
        *editField = EDIT_BOX_NONE;
        unpost_form(cons->forms.edit.form);
        post_form(cons->forms.search.form);
        set_current_field(cons->forms.search.form, cons->forms.search.fields[0]);
        int *client_socket = (int *) args[9];
        update_book(client_socket, cur_book);
    }
    clearAllWindow(args);
    return true;
}

bool cmd_KEY_F6(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_TITLE] = !checkbox[CHECKBOX_FILTER_BY_TITLE];
    }
    return true;
}

bool cmd_KEY_F7(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_AUTHOR] = !checkbox[CHECKBOX_FILTER_BY_AUTHOR];
    }
    return true;
}

bool cmd_KEY_F8(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_ANNOTATION] = !checkbox[CHECKBOX_FILTER_BY_ANNOTATION];
    }
    return true;
}

bool cmd_KEY_F9(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_TAG] = !checkbox[CHECKBOX_FILTER_BY_TAG];
    }
    return true;
}

bool cmd_KEY_F10(size_t *args) { return false; }

command knownCommands[] = {
        {DEFAULT_EVENT, &cmd_symbol},
        {KEY_UP_1,      &cmd_KEY_UP},
        {KEY_DOWN,      &cmd_KEY_DOWN},
        {KEY_F(1),      &cmd_KEY_F1},
        {KEY_F(2),      &cmd_KEY_F2},
        {KEY_F(3),      &cmd_KEY_F3},
        {KEY_F(4),      &cmd_KEY_F4},
        {KEY_F(5),      &cmd_KEY_F5},
        {KEY_F(6),      &cmd_KEY_F6},
        {KEY_F(7),      &cmd_KEY_F7},
        {KEY_F(8),      &cmd_KEY_F8},
        {KEY_F(9),      &cmd_KEY_F9},
        {KEY_F(10),     &cmd_KEY_F10},
        {KEY_MOUSE,     &cmd_KEY_MOUSE},
};

bool clientHadnler(int ch, size_t *args) { return event(ch, args, knownCommands, CI_SIZE(knownCommands)); }