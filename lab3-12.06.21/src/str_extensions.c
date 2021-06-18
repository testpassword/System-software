#include <string.h>
#include <malloc.h>
#include "../include/str_extensions.h"
#include <stdarg.h>

char* trim(char *str) {
    const char* sep = "\t\n\v\f\r ";
    size_t i = strlen(str) - 1;
    while (i >= 0 && strchr(sep, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    sep = "\t\n\v\f\r ";
    size_t totrim = strspn(str, sep);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) str[0] = '\0';
        else memmove(str, str + totrim, len + 1 - totrim);
    }
    return str;
}

int includes(char *source, char *find) {
    size_t N = strlen(source);
    size_t M = strlen(find);
    int isFound = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            if (source[i] == find[j]) {
                isFound = 1;
                i++;
            } else {
                isFound = 0;
                i += j - 1;
                break;
            }
        if (isFound) return isFound;
    }
    return -1;
}

int println(const char * format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);
    puts("");
    return ret;
}