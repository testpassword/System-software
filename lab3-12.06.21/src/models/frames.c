#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../../include/models/frames.h"
#include "../../include/errors.h"

int pack_frame(int socket, struct Frame *frame){
    unsigned char frame_array[2];
    frame_array[0] = frame->function;
    frame_array[1] = frame->function_parameter;
    if ((write(socket, frame_array, 2)) == -1) {
        perror("\nError: 'send message'");
        exit(1);
    }
    return 0;
}

int unpack_frame(int socket, struct Frame *frame) {
    unsigned char frame_array[2];
    long err = read(socket, frame_array, 2);
    if (err == -1) {
        fprintf(stderr, "Error read in unpack_frame");
        exit(1);
    }
    frame->function = frame_array[0];
    frame->function_parameter = frame_array[1];

//    switch (frame->function) {
//        case SERVER_FULL:
//            switch (frame->function_parameter) {
//                case 0:
//                    printf("Connection is established\n");
//                    break;
//                case 1:
//                    printf("Connection error: 'Server is full'\n");
//                    printf("Please retry later\n");
//                    break;
//                default:
//                    printf("Uknown function\n");
//            }
//            break;
//        case GET_ALL_BOOK:
//            printf(">>>Get All Book\n");
//            break;
//        case SEND_BOOK_EOF:
//            printf("Book EOF\n");
//            break;
//        case SEND_BOOK:
//            printf("Send Book\n");
//            break;
//        case CLIENT_QUIT:
//            printf("Client Quit\n");
//            break;
//        default:
//            printf("Error: 'Unknown function %d'\n", frame->function);
//            break;
//    }
    return 0;
}

int pack_book(int socket, struct BookFrame *frame) {
    if (send(socket, frame, sizeof(struct BookFrame), 0) == -1) {
        perror("\nError: 'send message'");
        return ERR_SERVER_SEND_BOOK;
    }
    return 0;
}

int unpack_book(int socket, struct BookFrame *frame) {
    if (read(socket, frame, sizeof(struct BookFrame)) < 1) {
        fprintf(stderr, "Error read in unpack_frame");
        exit(1);
    }
    return 0;
}
