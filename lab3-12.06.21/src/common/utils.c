#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
#include "../../include/common/utils.h"

char *ltrim(char *str, const char *sep) {
    if (sep == NULL) sep = "\t\n\v\f\r ";
    size_t totrim = strspn(str, sep);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) str[0] = '\0';
        else memmove(str, str + totrim, len + 1 - totrim);
    }
    return str;
}

char *rtrim(char *str, const char *sep) {
    if (sep == NULL) sep = "\t\n\v\f\r ";
    size_t i = strlen(str) - 1;
    while (i >= 0 && strchr(sep, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

char *trim(char *str) { return ltrim(rtrim(str, NULL), NULL); }

char *trim1(char *str, const char *sep) { return ltrim(rtrim(str, sep), sep); }

int seek_substring_KMP(char *source, char *find) {
    size_t N = strlen(source);
    size_t M = strlen(find);
    int succ = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++)
            if (source[i] == find[j]) {
                succ = 1;
                i++;
            } else {
                succ = 0;
                i += j - 1;
                break;
            }
        if(succ) return succ;
    }
    return -1;
}