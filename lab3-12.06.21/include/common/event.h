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

bool tryGetCmd(int ch, cmdFuncImpl *result, command *commandList, size_t commandListSize);
bool event(int ch, size_t *args, command *commandList, size_t commandListSize);

#endif