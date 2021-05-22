#ifndef __SWITCHS_H__
#define __SWITCHS_H__
#include <string.h>
#include <regex.h>
#include <stdbool.h>

// https://stackoverflow.com/questions/4014827/how-can-i-compare-strings-in-c-using-a-switch-statement

/** Begin a switch for the string x */
#define switchs(x) \
    { char *__sw = (x); bool __done = false; bool __cont = false; \
        regex_t __regex; regcomp(&__regex, ".*", 0); do {

/** Check if the string matches the cases argument (case sensitive) */
#define cases(x)    } if ( __cont || !strcmp ( __sw, x ) ) \
                        { __done = true; __cont = true;

/** Default behaviour */
#define defaults    } if ( !__done || __cont ) {

/** Close the switchs */
#define switchs_end } while ( 0 ); regfree(&__regex); }

#endif // __SWITCHS_H__