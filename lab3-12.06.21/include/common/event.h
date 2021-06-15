#ifndef SPO_LAB3_EVENT_H
#define SPO_LAB3_EVENT_H

#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_EVENT 0

typedef bool (*cmdFuncImpl)(size_t *);

typedef struct {
    int ch;
    cmdFuncImpl func;
} command;

/**
 *
 * @param ch
 * @param result -
 * @param commandList - store command operation. First element - default!
 * @param commandListSize - size of commandList with deafult element
 * @return
 */
bool tryGetCmd(int ch, cmdFuncImpl *result, command *commandList, size_t commandListSize);

/**
 *
 * @param ch
 * @param args
 * @param commandList
 * @param commandListSize
 * @return
 */
bool event(int ch, size_t *args, command *commandList, size_t commandListSize);

#endif //SPO_LAB3_EVENT_H
