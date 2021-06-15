#include <pthread.h>
#include "../../include/client/client.h"
#include "../../include/client/network.h"

int client_socket;

void *eventUpdate(void *args) {
    struct pthread_args_event *arg = args;
    int *ch = arg->ch;
    bool *running = arg->running;
    bool *needUpdate = arg->needUpdate;
    size_t *argsA = arg->args;
    while(*running) {
        *running = clientHadnler(*ch = getch(), argsA);
        *needUpdate = true;
    }
    return NULL;
}

int ui_work(struct book **books, int *count_book, bool *work, bool *needUpdate) {
    struct console console;
    if (initUI(&console)) return ERR_CLIENT_TERMINAL_ERRPR;
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
    pthread_create(&threadUpdateEvent, NULL, eventUpdate, &ev);
    do {
        if (*needUpdate)  { update(args); *needUpdate = false; }
        usleep(1);
    } while (running && *work);
    free(args);
    closeUI(&console);
    return SUCCESS;
}

void interrupt_close_client() {
    client_quit(&client_socket);
    exit(SIGINT);
}

void *threadUpdateBook(void *args) {
    struct pthread_args_client *arg = args;
    struct _config_frame configFrame;
    while(*(arg->connect)) {
        unpack(arg->connect_socket, &configFrame);
        if (configFrame.function == SEND_UPDATE_INFO) {
            get_books_net(&(arg->connect_socket), &(arg->books), arg->lenght);
            *(arg->needUpdate) = true;
        } else if(configFrame.function == SERVER_QUIT) *(arg->connect) = false;
    }
    return NULL;
}

int client(char *ip, long port) {
    signal(SIGINT, interrupt_close_client);
    int errno = connect_server(ip, port, &client_socket);
    if (errno > 0) return errno;
    int lenght = 0;
    struct book **books = calloc(1, sizeof(struct book *));
    get_books_net(&client_socket, &books, &lenght);
    bool work = true;
    bool needUpdate = true;
    pthread_t threadUpdate;
    struct pthread_args_client args = {
            .connect_socket = client_socket,
            .books = books,
            .lenght = &lenght,
            .connect = &work,
            .needUpdate = &needUpdate
    };
    pthread_create(&threadUpdate, NULL, threadUpdateBook, &args);
    errno = ui_work(books, &lenght, &work, &needUpdate);
    if(errno != SUCCESS) return errno;
    client_quit(&client_socket);
    free_books(books, lenght);
    return SUCCESS;
}