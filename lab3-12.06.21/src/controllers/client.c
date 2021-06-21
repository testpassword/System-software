#include <pthread.h>
#include "../../include/controllers/client.h"
#include <curses.h>
#include <string.h>
#include "../../include/str_extensions.h"
#include "../../include/dtos/Frame.h"

int client_socket;
const char* client_success = "Done!";

void *update_after_handle(void *args) {
    struct pthread_args_event *arg = args;
    int *ch = arg->ch;
    bool *running = arg->running;
    bool *needUpdate = arg->needUpdate;
    size_t *argsA = arg->args;
    while (*running) {
        *running = key_handle(*ch = getch(), argsA);
        *needUpdate = true;
    }
    return NULL;
}

int ui_listener(struct Book **books, int *count_book, bool *work, bool *needUpdate) {
    struct InputArea console;
    if (init(&console)) return ERR_CLIENT_TERMINAL_ERROR;
    int selectedBook = 0;
    int selectedPage = 0;
    int ch = 0;
    bool open_edit_form = false;
    bool checkboxFilter[4] = {false};
    int editField = EDIT_BOX_NONE;
    size_t *args = calloc(11, sizeof(size_t));
    args[0] = (size_t) &console;
    args[1] = (size_t) books;
    args[2] = (size_t) &selectedPage;
    args[3] = (size_t) &selectedBook;
    args[4] = (size_t) count_book;
    args[5] = (size_t) &open_edit_form;
    args[6] = (size_t) checkboxFilter;
    args[7] = (size_t) &ch;
    args[8] = (size_t) &editField;
    args[9] = (size_t) &client_socket;
    args[10] = (size_t) needUpdate;
    bool running = true;
    pthread_t threadUpdateEvent;
    struct pthread_args_event ev = {
            .running = &running,
            .ch = &ch,
            .args = args,
            .needUpdate = needUpdate
    };
    pthread_create(&threadUpdateEvent, NULL, update_after_handle, &ev);
    do {
        if (*needUpdate)  { redraw(args); *needUpdate = false; }
        usleep(1);
    } while (running && *work);
    free(args);
    destroy(&console);
    return SUCCESS;
}

void interrupt_close_client() {
    client_quit(&client_socket);
    exit(SIGINT);
}

void *update_book_thread_wrapper(void *args) {
    struct pthread_args_client *arg = args;
    struct Frame configFrame;
    while(*(arg->connect)) {
        unpack_frame(arg->connect_socket, &configFrame);
        if (configFrame.function == SEND_UPDATE_INFO) {
            get_books_net(&(arg->connect_socket), &(arg->books), arg->lenght);
            *(arg->needUpdate) = true;
        } else if (configFrame.function == SERVER_QUIT) *(arg->connect) = false;
    }
    return NULL;
}

int client(char *ip, long port) {
    signal(SIGINT, interrupt_close_client);
    int errno = connect_server(ip, port, &client_socket);
    if (errno > 0) return errno;
    int length = 0;
    struct Book **books = calloc(1, sizeof(struct Book *));
    get_books_net(&client_socket, &books, &length);
    bool work = true;
    bool needUpdate = true;
    pthread_t threadUpdate;
    struct pthread_args_client args = {
            .connect_socket = client_socket,
            .books = books,
            .lenght = &length,
            .connect = &work,
            .needUpdate = &needUpdate
    };
    pthread_create(&threadUpdate, NULL, update_book_thread_wrapper, &args);
    errno = ui_listener(books, &length, &work, &needUpdate);
    if(errno != SUCCESS) return errno;
    client_quit(&client_socket);
    burn_books(books, length);
    return SUCCESS;
}

void filter_listener(size_t *args) {
    struct InputArea *cons = (struct InputArea *) args[0];
    struct Book **books = (struct Book **) args[1];
    int *selected_page = ((int *) args[2]);
    int *selected_book = ((int *) args[3]);
    int *count_books = (int *) args[4];
    bool *checkbox = (bool *) args[6];
    form_driver(cons->forms.search.form, REQ_PREV_FIELD);
    form_driver(cons->forms.search.form, REQ_NEXT_FIELD);
    char *filter_req = trim(field_buffer(cons->forms.search.fields[0], 0));
    for (int i = 0; i < *count_books; i++) {
        struct Book* book = books[i];
        int title_filter = checkbox[CHECKBOX_FILTER_BY_TITLE] ? includes(book->title, filter_req) : false;
        int tag_filter = checkbox[CHECKBOX_FILTER_BY_TAG] ? includes(book->tags, filter_req) : false;
        int author_filter = checkbox[CHECKBOX_FILTER_BY_AUTHOR] ? includes(book->authors, filter_req) : false;
        int annotation_filter = checkbox[CHECKBOX_FILTER_BY_ANNOTATION] ? includes(book->annotation, filter_req) : false;
        if (title_filter == true || tag_filter == true || author_filter == true || annotation_filter == true) {
            if (i >= cons->text_area.books_list.book_lines) *selected_page = i / cons->text_area.books_list.book_lines;
            *selected_book = i % cons->text_area.books_list.book_lines;
            break;
        } else continue;
    }
}

bool cmd_symbol(size_t *args) {
    struct InputArea *cons = (struct InputArea *) args[0];
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
                filter_listener(args);
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

bool handle_UP(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!(*open_edit_form)) {
        struct InputArea *cons = (struct InputArea *) args[0];
        int lenghtArea = cons->text_area.books_list.book_lines;
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
        if ((*selectedPage) * lenghtArea + (*selectedBook) <= 0) (*selectedBook) = 0;
    }
    return true;
}

bool handle_DOWN(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!(*open_edit_form)) {
        struct InputArea *cons = (struct InputArea *) args[0];
        int lenghtArea = cons->text_area.books_list.book_lines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *lenght = (int *) args[4];
        int pages = (*lenght) / cons->text_area.books_list.book_lines + 1;
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

bool handle_F1(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct InputArea *cons = (struct InputArea *) args[0];
        struct Book **books = (struct Book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->text_area.books_list.book_lines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
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
        struct InputArea *cons = (struct InputArea *) args[0];
        struct Book **books = (struct Book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int lenghtArea = cons->text_area.books_list.book_lines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
        if (cur_book->available > 0) cur_book->available--;
        int *client_socket = (int *) args[9];
        update_book(client_socket, cur_book);
    }
    return true;
}

bool handle_F2(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct InputArea *cons = (struct InputArea *) args[0];
        struct Book **books = (struct Book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->text_area.books_list.book_lines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
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
        struct InputArea *cons = (struct InputArea *) args[0];
        struct Book **books = (struct Book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int lenghtArea = cons->text_area.books_list.book_lines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
        if (cur_book->available < cur_book->max) cur_book->available++;
        int *client_socket = (int *) args[9];
        update_book(client_socket, cur_book);
    }
    return true;
}

bool handle_F3(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct InputArea *cons = (struct InputArea *) args[0];
        struct Book **books = (struct Book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->text_area.books_list.book_lines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
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

bool handle_F4(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (*open_edit_form) {
        struct InputArea *cons = (struct InputArea *) args[0];
        struct Book **books = (struct Book **) args[1];
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int *editField = (int *) args[8];
        int lenghtArea = cons->text_area.books_list.book_lines;
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
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

bool handle_F5(size_t *args) {
    struct InputArea *cons = (struct InputArea *) args[0];
    bool *open_edit_form = (bool *) args[5];
    *open_edit_form = !(*open_edit_form);
    if (*open_edit_form) {
        unpost_form(cons->forms.search.form);
        post_form(cons->forms.edit.form);
        set_current_field(cons->forms.edit.form, cons->forms.edit.fields[0]);
    } else {
        struct Book **books = (struct Book **) args[1];
        int lenghtArea = cons->text_area.books_list.book_lines;
        int *selectedPage = ((int *) args[2]);
        int *selectedBook = ((int *) args[3]);
        int selectBook = (*selectedPage) * lenghtArea + (*selectedBook);
        struct Book *cur_book = books[selectBook];
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
    clear_window(args);
    return true;
}

bool handle_F6(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_TITLE] = !checkbox[CHECKBOX_FILTER_BY_TITLE];
    }
    return true;
}

bool handle_F7(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_AUTHOR] = !checkbox[CHECKBOX_FILTER_BY_AUTHOR];
    }
    return true;
}

bool handle_F8(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_ANNOTATION] = !checkbox[CHECKBOX_FILTER_BY_ANNOTATION];
    }
    return true;
}

bool handle_F9(size_t *args) {
    bool *open_edit_form = (bool *) args[5];
    if (!*open_edit_form) {
        bool *checkbox = (bool *) args[6];
        checkbox[CHECKBOX_FILTER_BY_TAG] = !checkbox[CHECKBOX_FILTER_BY_TAG];
    }
    return true;
}

bool handle_F10(size_t *args) { return false; }

Command knownCommands[] = {
        {DEFAULT_EVENT, &cmd_symbol},
        {KEY_UP_1, &handle_UP},
        {KEY_DOWN, &handle_DOWN},
        {KEY_F(1), &handle_F1},
        {KEY_F(2), &handle_F2},
        {KEY_F(3), &handle_F3},
        {KEY_F(4), &handle_F4},
        {KEY_F(5), &handle_F5},
        {KEY_F(6), &handle_F6},
        {KEY_F(7), &handle_F7},
        {KEY_F(8), &handle_F8},
        {KEY_F(9), &handle_F9},
        {KEY_F(10), &handle_F10}
};

bool key_handle(int ch, size_t *args) { return event(ch, args, knownCommands, CI_SIZE(knownCommands)); }

int open_socket(int *client_socket) {
    println("Setting connection...");
    *client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*client_socket < 0) {
        println("Problems with creating connection");
        return ERR_CLIENT_OPEN_SOCKET;
    } else {
        println(client_success);
        return SUCCESS;
    }
}

int connect_to_server(const int *client_socket, struct sockaddr_in *server_address) {
    println("Connecting to server...");
    if (connect(*client_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        println("Problems with connecting to server, try later");
        return ERR_CLIENT_CONNECT;
    } else {
        println(client_success);
        return SUCCESS;
    }
}

int check_connect(const int *client_socket) {
    struct Frame config_frame;
    unpack_frame(*client_socket, &config_frame);
    if (!((config_frame.function == ACCEPT_CONNECTION) && (config_frame.function_parameter == 0))) {
        close(*client_socket);
        return ERR_CLIENT_CONNECT_SERVER;
    } else return SUCCESS;
}

int connect_server(char *ip, long port, int *client_socket) {
    int err = SUCCESS;
    err = open_socket(client_socket);
    if (err != SUCCESS) return err;
    struct sockaddr_in server_address = {.sin_family = AF_INET, .sin_port = htons(port)};
    struct in_addr serv_address;
    if (inet_aton(ip, &serv_address) != 0) server_address.sin_addr = serv_address;
    else server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    err = connect_to_server(client_socket, &server_address);
    if (err != SUCCESS) return err;
    err = check_connect(client_socket);
    if (err != SUCCESS) return err;
    return SUCCESS;
}

void client_quit(const int *client_socket) {
    struct Frame configFrame = {.function = CLIENT_QUIT, .function_parameter = 0};
    pack_frame(*client_socket, &configFrame);
    println("Disconnecting from server...");
    shutdown(*client_socket, SHUT_RDWR);
    close(*client_socket);
    println(client_success);
}