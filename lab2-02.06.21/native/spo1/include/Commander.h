#pragma once
#include <stdio.h>
#include <blkid/blkid.h>
#include "HFSPlus.h"
#include "HFSPlusBTree.h"

#if defined(_MSC_VER)
//  Microsoft
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

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
PathListNode* EXPORT IterateDevices(FlexCommanderProbeInfo *info);
int EXPORT FlexOpen(const char * path, FlexCommanderFS* fs);
int EXPORT FlexListDirContent(const char* path, FlexCommanderFS* fs);
int EXPORT FlexSetCurrentDir(const char* path, FlexCommanderFS* fs);
int EXPORT FlexCopy(const char* path, const char* currentDir, FlexCommanderFS* fs);
#ifdef __cplusplus
}
#endif