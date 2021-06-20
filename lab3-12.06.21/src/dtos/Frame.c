#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../../include/errors.h"
#include "../../include/dtos/Frame.h"

int pack_frame(int socket, struct Frame *frame) {
    unsigned char frame_array[2];
    frame_array[0] = frame->function;
    frame_array[1] = frame->function_parameter;
    if ((write(socket, frame_array, 2)) == -1) {
        println("\nError: 'send message'");
        exit(1);
    }
    return 0;
}

int unpack_frame(int socket, struct Frame *frame) {
    unsigned char frame_array[2];
    if (read(socket, frame_array, 2) == -1) {
        println(stderr, "Error read in unpack_frame");
        exit(1);
    }
    frame->function = frame_array[0];
    frame->function_parameter = frame_array[1];
    return 0;
}