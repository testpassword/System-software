#include <string.h>
#include "../include/client/client.h"
#include "../include/server/server.h"
#include "../include/common/utils.h"
#include "../include/common/println.h"

#define LISTENER "--listen"
#define SERVE "--serve"

int main(int argc, char **argv) {
    const char* modes_tip = "поддерживаемые режимы: %s , %s";
    const char* network_tip = "для работы режима нужно передать IP адрес и порт в качестве аргументов";
    const char* port_tip = "порт задан некорректно: допустимы числа от 1 до 65535";
    if (argc == 1) {
        println(modes_tip, LISTENER, SERVE);
        return -1;
    }
    if (argc == 2) {
        if (strcmp(argv[1], LISTENER) == 0) println(network_tip);
        else if (strcmp(argv[1], SERVE) == 0) println("для работы режима необходимо задать порт");
        else println(modes_tip, LISTENER, SERVE);
        return -1;
    }
    if (argc == 3) {
        if(strcmp(argv[1], LISTENER) == 0) {
            println(network_tip);
        } else if (strcmp(argv[1], SERVE) == 0) {
            long port = strtol(argv[2], NULL, 10);
            if (port >= 65535 || port < 1) {
                println(port_tip);
                return -1;
            }
            return server(port);
        } else println(modes_tip, LISTENER, SERVE);
        return -1;
    }
    if (argc > 3) {
        if (strcmp(argv[1], LISTENER) == 0) {
            long port = strtol(argv[3], NULL, 10);
            if (port >= 65535 || port < 1) {
                println(port_tip);
                return -1;
            }
            if (!isValidIpAddress(argv[2])) {
                fprintf(stderr, "IP is incorrect");
                return -1;
            }
            return client(argv[2], port);
        } else if (strcmp(argv[1], SERVE) == 0) {
            fprintf(stderr, "Argument '%s' need only port\n", argv[1]);
            return -1;
        } else println(modes_tip, LISTENER, SERVE);
        return -1;
    }
    return -1;
}