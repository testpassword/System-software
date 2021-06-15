#ifndef SPO_LAB3_SERVEREVENT_H
#define SPO_LAB3_SERVEREVENT_H

#include "../common/event.h"
#include "../common/protocol.h"

#include <stdbool.h>
#include <stdlib.h>

#define CI_SIZE(s) (sizeof(s)/sizeof(command))

bool serverHandler(int ch, size_t *args);

#endif //SPO_LAB3_SERVEREVENT_H
