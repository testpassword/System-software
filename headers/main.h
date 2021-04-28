#pragma once

#define SYS_BLOCK_DIR "/sys/block/" //https://www.kernel.org/doc/html/latest/block/stat.html
#define DRIVE_PATTERN "^((sd|hd)[a-z])|(nvme[0-9])"

int partitionsMode();

int shellMode(char *name);