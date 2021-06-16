#include <string.h>
#include <stdlib.h>
#include "../include/client/client.h"
#include "../include/server.h"
#include "../include/common/utils.h"
#include "../include/println.h"

#define CONSUMER "--client"
#define PRODUCER "--serve"

int validate_port_or_exit(long p) {
    if (p >= 65535 || p < 1) {
        println("порт задан некорректно: допустимы числа от 1 до 65535");
        exit(-1);
    }
}

bool validate_IP_or_exit(char* ipAddress) {
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ipAddress, &(sa.sin_addr)) == 0) {
        println("IP задан неправильно: проверять его на соответствие правилам IPv4");
        exit(-1);
    }
}

int main(int argc, char **argv) {
    const char* modes_tip = "поддерживаемые режимы: %s , %s";
    const char* network_tip = "для работы режима нужно передать IP адрес и порт в качестве аргументов";
    if (argc == 1) {
        println(modes_tip, CONSUMER, PRODUCER);
        return -1;
    }
    if (argc == 2) {
        if (strcmp(argv[1], CONSUMER) == 0) println(network_tip);
        else if (strcmp(argv[1], PRODUCER) == 0) println("для работы режима необходимо задать порт");
        else println(modes_tip, CONSUMER, PRODUCER);
        return -1;
    }
    if (argc == 3) {
        if(strcmp(argv[1], CONSUMER) == 0) println(network_tip);
        else if (strcmp(argv[1], PRODUCER) == 0) {
            long port = strtol(argv[2], NULL, 10);
            validate_port_or_exit(port);
            return serve(port);
        } else println(modes_tip, CONSUMER, PRODUCER);
        return -1;
    }
    if (argc > 3) {
        if (strcmp(argv[1], CONSUMER) == 0) {
            long port = strtol(argv[3], NULL, 10);
            validate_port_or_exit(port);
            validate_IP_or_exit(argv[2]);
            return client(argv[2], port);
        } else if (strcmp(argv[1], PRODUCER) == 0) {
            fprintf(stderr, "в режиме сервера необходимо задать только порт в качестве параметров");
            return -1;
        } else println(modes_tip, CONSUMER, PRODUCER);
        return -1;
    }
    return -1;
}