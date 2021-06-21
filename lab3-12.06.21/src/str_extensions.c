#include <string.h>
#include <malloc.h>
#include "../include/str_extensions.h"
#include <stdarg.h>
#include <stdbool.h>

// https://cboard.cprogramming.com/c-programming/173158-java-style-println-c.html
int println(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);
    puts("");
    return ret;
}

char* trim(char* str) {
    const char* sep = "\t\n\v\f\r ";
    size_t i = strlen(str) - 1;
    while (i >= 0 && strchr(sep, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    size_t totrim = strspn(str, sep);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) str[0] = '\0';
        else memmove(str, str + totrim, len + 1 - totrim);
    }
    return str;
}

bool includes(char* source, char* substring) {
    if (strlen(substring) < 1 || strlen(source) < 1) return -1;
    return strstr(source, substring) != NULL ? 0 : -1;
}