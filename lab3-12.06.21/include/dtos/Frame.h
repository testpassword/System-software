struct Frame {
    unsigned char function;
    unsigned char function_parameter;
};

int pack_frame(int socket, struct Frame *frame);
int unpack_frame(int socket, struct Frame *frame);