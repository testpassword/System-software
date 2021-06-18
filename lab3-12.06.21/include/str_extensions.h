#ifndef SPO_LAB3_UTILS_H
#define SPO_LAB3_UTILS_H

#include <stdbool.h>

char* trim(char *str);
int includes(char *source, char *find);
int println(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

#endif
