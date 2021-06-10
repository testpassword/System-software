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

typedef struct ProbeInfo {
    blkid_cache blkCache;
} ProbeInfo;

typedef struct FileSystem {
    FILE *file;
    uint32_t blockSize;
    uint64_t catalogFileBlock;
    HFSPlusVolumeHeader volumeHeader;
    PathListNode* output;
} FileSystem;

#ifdef __cplusplus
extern "C" {
#endif
//Functions for block device probing
int EXPORT Init(ProbeInfo *info);
PathListNode* EXPORT IterateDevices(ProbeInfo *info);
int EXPORT openByPath(const char * path, FileSystem* fs);
int EXPORT dirContent(const char* path, FileSystem* fs);
int EXPORT setCurDir(const char* path, FileSystem* fs);
int EXPORT copyItems(const char* path, const char* currentDir, FileSystem* fs);
#ifdef __cplusplus
}
#endif