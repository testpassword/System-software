#pragma once

#include <stdio.h>
#include <blkid/blkid.h>
#include "HFSPlus.h"
#include "HFSPlusBTree.h"
//#include "../wrapper/include/List.h"

#if defined(_MSC_VER)
//  Microsoft
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

//#include "../wrapper/include/List.h"

typedef struct PathListNode PathListNode;

typedef struct FlexCommanderProbeInfo {
    blkid_cache blkCache;
} FlexCommanderProbeInfo;

typedef struct FlexCommanderFS {
    FILE *file;
    uint32_t blockSize;
    uint64_t catalogFileBlock;
    HFSPlusVolumeHeader volumeHeader;
    PathListNode* output;
} FlexCommanderFS;

#ifdef __cplusplus
extern "C" {
#endif
//Functions for block device probing
int EXPORT Init(FlexCommanderProbeInfo *info);
int EXPORT ProbeDevices(FlexCommanderProbeInfo *info);
PathListNode* EXPORT IterateDevices(FlexCommanderProbeInfo *info);
//Functions for filesystem interaction
int EXPORT FlexOpen(const char * path, FlexCommanderFS* fs);
int EXPORT FlexListDirContent(const char* path, FlexCommanderFS* fs);
int EXPORT FlexSetCurrentDir(const char* path, FlexCommanderFS* fs);
int EXPORT FlexCopy(const char* path, const char* currentDir, FlexCommanderFS* fs);
#ifdef __cplusplus
}
#endif