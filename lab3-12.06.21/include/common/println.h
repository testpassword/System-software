#include <stdarg.h>

// https://cboard.cprogramming.com/c-programming/173158-java-style-println-c.html
#ifdef __GNUC__
int println(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#endif
 
int println(const char * format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);
    puts("");
    return ret;
}