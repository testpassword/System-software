#include <stdbool.h>

char* trim(char *str);
bool includes(char *source, char *substring);
int println(const char *format, ...) __attribute__ ((format (printf, 1, 2)));