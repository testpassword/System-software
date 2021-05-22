#pragma once
#include <stddef.h>
#include "hfsplus_structs.h"

typedef struct File {
    long size;
    void* data;
} File;

typedef struct BTree {
    File* file;
    BTHeaderRec* header;
} BTree;

typedef struct FileSystem {
    int descriptor;
    HFSPlusVolumeHeader* header;
    BTree* catalog;
    HFSCatalogNodeID pwd;
} FileSystem;

typedef struct Node {
    BTNodeDescriptor* descriptor;
    UInt16* record_offsets;
} Node;

typedef struct Record {
    void* data;
    UInt16 size;
    UInt16 data_offset;
    UInt16 key_length;
} Record;

typedef struct IterationData {
    Record* record;
    SInt16 type;
    HFSCatalogNodeID parent_id;
    HFSCatalogNodeID target_id;
    char node_name[256];
    char target_name[256];
    char break_flag;
} IterationData;

typedef struct NodeInfo {
    HFSCatalogNodeID id;
    SInt16 type;
    char name[256];
    HFSPlusForkData data;
} NodeInfo;

typedef struct NodeInfoArray {
    int size;
    NodeInfo* data;
} NodeInfoArray;