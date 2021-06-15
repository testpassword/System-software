#ifndef SPO_LAB3_CLIENTEVENT_H
#define SPO_LAB3_CLIENTEVENT_H

#include <stdbool.h>
#include <stdlib.h>
#include "../common/event.h"

#define CI_SIZE(s) (sizeof(s)/sizeof(command))

#define KEY_UP_1 259
#define KEY_DEL 127
#define KEY_NL 10

bool clientHadnler(int ch, size_t *args);

bool cmd_symbol(size_t *args);

bool cmd_KEY_UP(size_t *args);

bool cmd_KEY_DOWN(size_t *args);

bool cmd_KEY_F1(size_t *args);

bool cmd_KEY_F2(size_t *args);

bool cmd_KEY_F3(size_t *args);

bool cmd_KEY_F4(size_t *args);

bool cmd_KEY_F5(size_t *args);

bool cmd_KEY_F6(size_t *args);

bool cmd_KEY_F7(size_t *args);

bool cmd_KEY_F8(size_t *args);

bool cmd_KEY_F9(size_t *args);

bool cmd_KEY_F10(size_t *args);

bool cmd_KEY_MOUSE(size_t *args);

#endif //SPO_LAB3_CLIENTEVENT_H
