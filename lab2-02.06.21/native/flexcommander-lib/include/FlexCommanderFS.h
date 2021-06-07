#pragma once

typedef struct FlexCommanderFS {
    FILE *file;
    uint32_t blockSize;
    uint64_t catalogFileBlock;
    HFSPlusVolumeHeader volumeHeader;
    PathListNode* output;
} FlexCommanderFS;
